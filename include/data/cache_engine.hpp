#ifndef CACHE_ENGINE_HPP
#define CACHE_ENGINE_HPP

#include "hashmap.hpp"
#include "lru.hpp"
#include "ttl_heap.hpp"
#include "graph.hpp"
#include <optional>
#include <string>
#include <vector>

using namespace std;

class CacheEngine {

public:
    CacheEngine(size_t capacity = 1000);

    void set (const string& key, const string& value, int ttl_seconds = 0);
    optional<string> get (const string& key);
    bool del(const string& key);
    bool expire(const string& key, int ttl_seconds);

    void depend(const string& parent, const string& child);

    size_t size() const;

private: 

    HashMap<string, string> store_;
    LRUCache <string, string> lru_;
    TTLHeap <string, string> ttl_;
    Graph deps_;

    void purgeExpired();
    void cascadeDelete(const string& key);
    
};

#endif