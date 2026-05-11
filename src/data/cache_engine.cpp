#include "data/cache_engine.hpp"
#include "analytics/topk.hpp"
#include <iostream>
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>
using namespace std;

#include "data/cache_engine_state.hpp"

CacheEngine::CacheEngine(): lru_(100) {}

CacheEngine::CacheEngine(size_t capacity)
    : lru_(capacity) {}

void CacheEngine::set(const string& key, const string& value) {
    cleanupExpiredKeys();
    hashmap_.insert(key, value);
    bloom_.insert(key);
    trie_.insert(key);
    ttl_heap_.erase(key);

    auto evicted = lru_.put(key, value);
    if (evicted.has_value())
        deleteCascade(evicted.value());
}

void CacheEngine::set(const string& key, const string& value, int ttl_seconds) {
    cleanupExpiredKeys();
    hashmap_.insert(key, value);
    bloom_.insert(key);
    trie_.insert(key);
    ttl_heap_.erase(key);
    ttl_heap_.insert(key, value, ttl_seconds);

    auto evicted = lru_.put(key, value);
    if (evicted.has_value())
        deleteCascade(evicted.value());
}

optional<string> CacheEngine::get(const string& key) {
    cleanupExpiredKeys();
    if (!bloom_.possiblyExists(key))
        return nullopt;

    auto valueOpt= hashmap_.get(key);
    if (valueOpt.has_value()) {
        freqMap_[key]++;
        auto evicted = lru_.put(key, valueOpt.value());
        if (evicted.has_value())
            deleteCascade(evicted.value());
        return valueOpt.value();
    }
    return nullopt;
}

void CacheEngine::del(const string& key) {
    cleanupExpiredKeys();
    deleteCascade(key);
}

void CacheEngine::expire(const string& key, int ttl_seconds) {
    cleanupExpiredKeys();
    auto valueOpt = hashmap_.get(key);
    if (valueOpt.has_value()) {
        ttl_heap_.erase(key);
        ttl_heap_.insert(key, valueOpt.value(), ttl_seconds);
    }
}

void CacheEngine::link(const string& from, const string& to) {
    graph_.addDependency(from, to);
}

void CacheEngine::depend(const string& parent, const string& child) {
    graph_.addDependency(parent, child);
}

void CacheEngine::cleanupExpiredKeys() {
    vector<string> expired= ttl_heap_.removeExpired();
    unordered_set<string> uniq(expired.begin(), expired.end());
    for (const auto& key : uniq)
        deleteCascade(key);
}
void CacheEngine::deleteCascade(const string& key) {
    unordered_set<string> to_delete;
    auto deps = graph_.getAllDependentsRecursive(key);
    to_delete.insert(deps.begin(), deps.end());
    to_delete.insert(key);

    for (const auto& k : to_delete) {
        hashmap_.erase(k);
        lru_.erase(k);
        trie_.remove(k);
        ttl_heap_.erase(k);
        freqMap_.erase(k);
        graph_.removeNode(k);
    }
}

bool CacheEngine::trieSearch(const string& key) const {
    return trie_.search(key);
}

vector<string> CacheEngine::triePrefix(const string& p) const {
    return prefix(p);
}

vector<string> CacheEngine::prefix(const string& p) const {
    vector<string> result= trie_.getWordsWithPrefix(p);

    // filter results using hashmap (BF may have false positives)
    vector<string> finalList;
    for (auto& k: result) {
        if (hashmap_.contains(k)) 
            finalList.push_back(k);
    }

    return finalList;
}

size_t CacheEngine::size() const {
    return hashmap_.size();

}

vector<pair<string,int>> CacheEngine::topK(int k) const {
    TopK tk;
    return tk.computeTopK(freqMap_, k);
}

CacheEngineState CacheEngine::exportState() {
    cleanupExpiredKeys();
    CacheEngineState st;
    vector<string> keys = hashmap_.keys();
    unordered_set<string> keyset(keys.begin(), keys.end());

    auto ttl_seconds = ttl_heap_.getRemainingSeconds();
    auto ttl_epoch = ttl_heap_.getExpiryEpochMs();

    for (const auto &k : keys) {
        auto valOpt = hashmap_.get(k);
        if (!valOpt.has_value()) continue;

        CacheEntryState e;
        e.key = k;
        e.value = valOpt.value();
        auto it = ttl_seconds.find(k);
        e.ttl = (it != ttl_seconds.end()) ? it->second : -1;
        e.status = "Synced";
        st.entries.push_back(e);
    }

    for (const auto &k : keys) {
        vector<string> deps = graph_.getDependents(k);
        for (const auto &child : deps) {
            if (!keyset.count(child)) continue;
            CacheLinkState l;
            l.parent = k;
            l.child = child;
            st.links.push_back(l);
        }
    }

    vector<string> sorted = keys;
    sort(sorted.begin(), sorted.end());
    st.skiplist = sorted;

    st.lru = lru_.getOrder();
    st.ttl_expiry = std::move(ttl_epoch);
    for (auto it = st.ttl_expiry.begin(); it != st.ttl_expiry.end(); ) {
        if (!keyset.count(it->first)) it = st.ttl_expiry.erase(it);
        else ++it;
    }

    st.topk = last_topk_k_ > 0 ? topK(last_topk_k_) : vector<pair<string,int>>{};
    st.trie_matches = last_prefix_query_.empty() ? vector<string>{} : prefix(last_prefix_query_);

    auto buckets = hashmap_.buckets();
    st.hashmap_buckets.reserve(buckets.size());
    for (const auto& bucket : buckets) {
        vector<string> out;
        out.reserve(bucket.size());
        for (const auto& kv : bucket) {
            out.push_back(kv.first + ":" + kv.second);
        }
        st.hashmap_buckets.push_back(std::move(out));
    }

    return st;
}

void CacheEngine::clear() {
    hashmap_.clear();
    lru_.clear();
    ttl_heap_.clear();
    graph_.clear();
    trie_.clear();
    bloom_.clear();
    freqMap_.clear();
    last_prefix_query_.clear();
    last_topk_k_ = 5;
}

bool CacheEngine::contains(const string& key) const {
    return hashmap_.contains(key);
}

void CacheEngine::cleanupExpired() {
    cleanupExpiredKeys();
}

void CacheEngine::setLastPrefixQuery(const string& prefix) {
    last_prefix_query_ = prefix;
}

void CacheEngine::setLastTopK(int k) {
    last_topk_k_ = k;
}
