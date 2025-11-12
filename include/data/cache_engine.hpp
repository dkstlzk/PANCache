#pragma once
#include <string>
#include <optional>
#include "data/hashmap.hpp"
#include "data/lru.hpp"
#include "data/ttl_heap.hpp"
#include "depend/graph.hpp"

using namespace std;

class CacheEngine {
public:
    // Constructors
    CacheEngine();                  
    explicit CacheEngine(size_t capacity);  

    // Core cache operations
    void set(const string& key, const string& value);
    void set(const string& key, const string& value, int ttl_seconds);

    std::optional<string> get(const string& key);
    void del(const string& key);
    void expire(const string& key, int ttl_seconds);
    void link(const string& from, const string& to);
    void depend(const string& parent, const string& child); // for test compatibility

    size_t size() const;

private:
    LRUCache<string, string> lru_;
    HashMap<string, string> hashmap_;
    TTLHeap<string, string> ttl_heap_;
    Graph graph_;
};
