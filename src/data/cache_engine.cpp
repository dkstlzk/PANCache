#include "data/cache_engine.hpp"
#include <iostream>
#include <optional>
#include <string>

CacheEngine::CacheEngine() : lru_(100) {}

CacheEngine::CacheEngine(size_t capacity)
    : lru_(capacity) {}

void CacheEngine::set(const std::string& key, const std::string& value) {
    hashmap_.insert(key, value);
    lru_.put(key, value);
}

void CacheEngine::set(const std::string& key, const std::string& value, int ttl_seconds) {
    hashmap_.insert(key, value);
    lru_.put(key, value);
    ttl_heap_.insert(key, value, ttl_seconds);
}

std::optional<std::string> CacheEngine::get(const std::string& key) {
    auto valueOpt = hashmap_.get(key);
    if (valueOpt.has_value()) {
        lru_.put(key, valueOpt.value());
        return valueOpt.value();
    }
    return std::nullopt;
}

void CacheEngine::del(const std::string& key) {
    hashmap_.erase(key);
    lru_.erase(key);
}

void CacheEngine::expire(const std::string& key, int ttl_seconds) {
    auto valueOpt = hashmap_.get(key);
    if (valueOpt.has_value()) {
        ttl_heap_.insert(key, valueOpt.value(), ttl_seconds);
    }
}

void CacheEngine::link(const std::string& from, const std::string& to) {
    graph_.addDependency(from, to);
}

void CacheEngine::depend(const std::string& parent, const std::string& child) {
    graph_.addDependency(parent, child);
}

size_t CacheEngine::size() const {
    return hashmap_.size();
}
