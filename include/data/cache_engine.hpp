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

#include "data/cache_engine_state.hpp"

class CacheEngine {
public:
    CacheEngine();                  
    explicit CacheEngine(std::size_t capacity);  

    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, const std::string& value, int ttl_seconds);

    std::optional<std::string> get(const std::string& key);
    void del(const std::string& key);
    void expire(const std::string& key, int ttl_seconds);
    void link(const std::string& from, const std::string& to);
    void depend(const std::string& parent, const std::string& child); 
    void clear();
    bool contains(const std::string& key) const;
    void cleanupExpired();

    bool trieSearch(const std::string& key) const;
    std::vector<std::string> triePrefix(const std::string& prefix) const;

    std::size_t size() const;
    std::vector<std::string> prefix(const std::string& p) const;
    std::vector<std::pair<std::string,int>> topK(int k) const;
    CacheEngineState exportState();

    void setLastPrefixQuery(const std::string& prefix);
    void setLastTopK(int k);

private:
    LRUCache<std::string, std::string> lru_;
    HashMap<std::string, std::string> hashmap_;
    TTLHeap<std::string, std::string> ttl_heap_;
    PANCache::Depend::Graph graph_;
    Trie trie_;  
    void cleanupExpiredKeys();
    void deleteCascade(const std::string& key);
    BloomFilter bloom_; 
    std::unordered_map<std::string, int> freqMap_;
    std::string last_prefix_query_;
    int last_topk_k_ = 5;
};
