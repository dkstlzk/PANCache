#include "data/cache_engine.hpp"
#include <iostream>

CacheEngine::CacheEngine (size_t capacity)
    : store_(), lru_(static_cast<int>(capacity)), ttl_(), deps_() {}

void CacheEngine::purgeExpired() {

    auto removed = ttl_.removeExpired();
    for (const auto &k : removed) {
        store_.erase(k);
        lru_.erase(k);
        deps_.removeNode(k);
    }
}

void CacheEngine::cascadeDelete( const string &key) {

    if (!deps_.hasNode(key)) {
        store_.erase(key);
        lru_.erase(key);
        deps_.removeNode(key);
        return;
    }

    auto reachable = deps_.getAllReachable(key);
    for (const auto &k : reachable) {
        store_.erase(k);
        lru_.erase(k);
        deps_.removeNode(k);
    }
}

void CacheEngine::set (const string& key, const string& value, int ttl_seconds) {

    purgeExpired();
    store_.insert(key, value);
    lru_.put(key, value);
    deps_.addNode(key);
    if (ttl_seconds > 0) {
        ttl_.insert(key, value, ttl_seconds);
    }

}

optional <string> CacheEngine::get(const string& key) {
    purgeExpired();
    auto v = store_.get(key);
    if (!v.has_value()) return nullopt;
    lru_.get(key);
    return v;
}

bool CacheEngine::del(const string& key) {
    
    purgeExpired();
    if (!store_.contains(key)) return false;
    cascadeDelete(key);
    store_.erase(key);
    lru_.erase(key);
    deps_.removeNode(key);
    return true;
}

bool CacheEngine::expire(const string& key, int ttl_seconds) {

    purgeExpired ();
    auto v = store_.get(key);
    if (!v.has_value()) return false;
    ttl_.insert(key, v.value(), ttl_seconds);
    return true;
}

void CacheEngine::depend(const string& parent, const string& child) {

    deps_.addNode(parent );
    deps_.addNode(child);
    deps_.addEdge(parent, child);
}

size_t CacheEngine::size() const {
    return store_.size();
}
