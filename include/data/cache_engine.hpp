#pragma once
#include <string>
#include <optional>
#include "data/hashmap.hpp"
#include "data/lru.hpp"
#include "data/ttl_heap.hpp"
#include "depend/graph.hpp"
//#include "../depend/graph.hpp"
#include "data/trie.hpp"
#include "data/bloom_filter.hpp"  
#include "analytics/topk.hpp"


using namespace std;

class CacheEngine {
public:
    CacheEngine();                  
    explicit CacheEngine(size_t capacity);  

    void set(const string& key, const string& value);
    void set(const string& key, const string& value, int ttl_seconds);

    std::optional<string> get(const string& key);
    void del(const string& key);
    void expire(const string& key, int ttl_seconds);
    void link(const string& from, const string& to);
    void depend(const string& parent, const string& child); 

    bool trieSearch(const string& key) const;
    vector<string> triePrefix(const string& prefix) const;

    size_t size() const;
    vector<string> prefix(const string& p) const;
    vector<pair<string,int>> topK(int k) const;


private:
    LRUCache<string, string> lru_;
    HashMap<string, string> hashmap_;
    TTLHeap<string, string> ttl_heap_;
    PANCache::Depend::Graph graph_;
    Trie trie_;  
    void cleanupExpiredKeys();
    void deleteCascade(const string& key);
    BloomFilter bloom_; 
    unordered_map<string, int> freqMap_;
};
