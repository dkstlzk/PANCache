#include "data/cache_engine.hpp"
#include <iostream>
#include <optional>
#include <string>

CacheEngine::CacheEngine() : lru_(100) {}

CacheEngine::CacheEngine(size_t capacity)
    : lru_(capacity) {}

// Insert without TTL
void CacheEngine::set(const std::string& key, const std::string& value) {
    hashmap_.insert(key, value);
    lru_.put(key, value);
}

// Insert with TTL
void CacheEngine::set(const std::string& key, const std::string& value, int ttl_seconds) {
    hashmap_.insert(key, value);
    lru_.put(key, value);
    ttl_heap_.insert(key, value, ttl_seconds);
}

// Retrieve a value if exists
std::optional<std::string> CacheEngine::get(const std::string& key) {
    auto valueOpt = hashmap_.get(key);
    if (valueOpt.has_value()) {
        lru_.put(key, valueOpt.value()); // mark as recently used
        return valueOpt.value();
    }
    return std::nullopt;
}

// Delete key from cache
void CacheEngine::del(const std::string& key) {
    hashmap_.erase(key);
    lru_.erase(key);
}

// Set TTL expiry for key
void CacheEngine::expire(const std::string& key, int ttl_seconds) {
    auto valueOpt = hashmap_.get(key);
    if (valueOpt.has_value()) {
        ttl_heap_.insert(key, valueOpt.value(), ttl_seconds);
    }
}

// Link dependency between two keys
void CacheEngine::link(const std::string& from, const std::string& to) {
    graph_.addDependency(from, to);
}

// Alias for tests (same as link)
void CacheEngine::depend(const std::string& parent, const std::string& child) {
    graph_.addDependency(parent, child);
}

// Return number of entries in cache
size_t CacheEngine::size() const {
    return hashmap_.size();
}
