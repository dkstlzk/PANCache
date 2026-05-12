#ifndef HASHMAP_HPP
#define HASHMAP_HPP

#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <utility>

template <typename K, typename V>
class HashMap {
private:
    struct Entry {
        K key;
        V value;
        Entry* next;
        Entry(const K& k, const V& v): key(k), value(v), next(nullptr) {}
    };

    std::size_t capacity;         
    std::size_t count;
    std::vector<Entry*> table;

    const float load_factor=0.75f;

    std::size_t hashKey(const K& key) const { return std::hash<K>{}(key) % capacity; }
    void resizeIfNeeded();

public:
    HashMap();                          // default: capacity = 16
    explicit HashMap(std::size_t initial_capacity);
    ~HashMap();

    void insert(const K& key, const V& value);
    std::optional<V> get(const K& key) const;
    bool erase(const K& key);
    bool contains(const K& key) const;

    std::size_t size() const { return count; }
    void clear();
    std::vector<K> keys() const;
    std::vector<std::vector<std::pair<K, V>>> buckets() const;
};

#endif 
