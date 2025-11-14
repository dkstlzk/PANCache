#include "data/cache_engine.hpp"
#include "data/bloom_filter.hpp"
#include "data/trie.hpp"

#include <iostream>
#include <optional>
#include <string>
using namespace std;

CacheEngine::CacheEngine() : 
    lru_(100),
    bloom_(1000)  
{}

CacheEngine::CacheEngine(size_t capacity)
    : lru_(capacity),
      bloom_(capacity * 10) 
{}

void CacheEngine::set(const string& key, const string& value) {
    bloom_.insert(key);
    hashmap_.insert(key, value);
    lru_.put(key, value);
    trie_.insert(key);   
}

void CacheEngine::set(const string& key, const string& value, int ttl_seconds) {
    bloom_.insert(key);
    hashmap_.insert(key, value);
    lru_.put(key, value);
    ttl_heap_.insert(key, value, ttl_seconds);
    trie_.insert(key);
}

optional<string> CacheEngine::get(const string& key) {
    if(!bloom_.possiblyExists(key)) {
        return nullopt;        
    }

    auto valueOpt = hashmap_.get(key);
    if (valueOpt.has_value()) {
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
    auto valueOpt = hashmap_.get(key);
    if(valueOpt.has_value()) {
        ttl_heap_.insert(key, valueOpt.value(), ttl_seconds);
    }
}

void CacheEngine::link(const string& from, const string& to) {
    graph_.addDependency(from, to);
}

void CacheEngine::depend(const string& parent, const string& child) {
    graph_.addDependency(parent, child);
}

size_t CacheEngine::size() const {
    return hashmap_.size();
}

vector<string> CacheEngine::prefix(const string& p) const {
    return trie_.getWordsWithPrefix(p);
}


