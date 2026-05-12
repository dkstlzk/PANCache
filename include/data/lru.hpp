#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include "hashmap.hpp"
#include <optional>
#include <vector>
template <typename K, typename V>
class LRUCache {
private:
    struct Node {
        K key;
        V value;
        Node* prev;
        Node* next;
        Node(K k, V v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    int capacity;
    int count;
    Node* head;
    Node* tail;

    HashMap<K, void*> cacheMap;

    void moveToFront(Node* node);
    void removeNode(Node* node);
    void addToFront (Node* node);

public:
    explicit LRUCache(int cap);
    ~LRUCache();

    std::optional<K> put(const K& key, const V& value);
    std::optional<V> get(const K& key);
    void display() const;
    bool erase(const K& key);
    std::size_t size() const { return count; }
    std::vector<K> getOrder() const;
    void clear();
};

#endif