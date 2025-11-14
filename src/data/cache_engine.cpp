#include "data/cache_engine.hpp"
#include "analytics/topk.hpp"
#include <iostream>
#include <optional>
#include <string>
using namespace std;

CacheEngine::CacheEngine(): lru_(100) {}

CacheEngine::CacheEngine(size_t capacity)
    : lru_(capacity) {}

void CacheEngine::set(const string& key, const string& value) {
    hashmap_.insert(key, value);
    lru_.put(key, value);
    trie_.insert(key);    
}

void CacheEngine::set(const string& key, const string& value, int ttl_seconds) {
    cleanupExpiredKeys();
    hashmap_.insert(key, value);
    lru_.put(key, value);
    ttl_heap_.insert(key, value, ttl_seconds);
    trie_.insert(key); 
}

optional<string> CacheEngine::get(const string& key) {
    cleanupExpiredKeys();
    auto valueOpt= hashmap_.get(key);
    if (valueOpt.has_value()) {
        freqMap_[key]++;
        lru_.put(key, valueOpt.value());
        return valueOpt.value();
    }
    return nullopt;
}

void CacheEngine::del(const string& key) {
    hashmap_.erase(key);
    lru_.erase(key);
}

void CacheEngine::expire(const string& key, int ttl_seconds) {
    cleanupExpiredKeys();
    auto valueOpt = hashmap_.get(key);
    if (valueOpt.has_value()) {
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
    for (auto& key : expired) {
        deleteCascade(key);
    }
}
void CacheEngine::deleteCascade(const string& key) {

    vector<string> all= graph_.getAllDependentsRecursive(key);

    // delete all dependent keys
    for (auto& k : all) {
        hashmap_.erase(k);
        lru_.erase(k);
        trie_.remove(k);    
        // TTLHeap already removed them if TTL expiry triggered
    }

    // 3. delete the key itself
    hashmap_.erase(key);
    lru_.erase(key);
    trie_.remove(key);      
}

bool CacheEngine::trieSearch(const string& key) const {
    return trie_.search(key);
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

