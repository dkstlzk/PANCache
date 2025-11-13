#ifndef HASHMAP_HPP
#define HASHMAP_HPP

#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <utility>

using namespace std;

template <typename K, typename V>
class HashMap {
private:
    struct Entry {
        K key;
        V value;
        Entry* next;
        Entry(const K& k, const V& v): key(k), value(v), next(nullptr) {}
    };

    size_t capacity;         
    size_t count;
    vector<Entry*> table;

    const float load_factor=0.75f;

    size_t hashKey(const K& key) const { return hash<K>{}(key)%capacity; }
    void resizeIfNeeded();

public:
    HashMap();                          // default: capacity = 16
    explicit HashMap(size_t initial_capacity);
    ~HashMap();

    void insert(const K& key, const V& value);
    optional<V> get(const K& key) const;
    bool erase(const K& key);
    bool contains(const K& key) const;

    size_t size() const { return count; }
    void clear();
};

#endif 
