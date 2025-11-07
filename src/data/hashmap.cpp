#include "data/hashmap.hpp"

// default constructor
template <typename K, typename V>
HashMap<K, V>::HashMap()
    : capacity(16), count(0), table(16, nullptr) {}

// parameterized constructor
template <typename K, typename V>
HashMap<K, V>::HashMap(size_t initial_capacity)
    : capacity(initial_capacity), count(0), table(initial_capacity, nullptr) {}

// destructor
template <typename K, typename V>
HashMap<K, V>::~HashMap() {
    clear();
}

// clear all entries
template <typename K, typename V>
void HashMap<K, V>::clear() {
    for (auto& head : table) {
        Entry* curr = head;
        while (curr) {
            Entry* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
        head = nullptr;
    }
    count = 0;
}

// resize when load factor exceeded
template <typename K, typename V>
void HashMap<K, V>::resizeIfNeeded() {
    if (static_cast<float>(count) / static_cast<float>(capacity) < load_factor)
        return;

    size_t new_capacity = capacity * 2;
    vector<Entry*> new_table(new_capacity, nullptr);

    for (auto& head : table) {
        Entry* curr = head;
        while (curr) {
            Entry* next = curr->next;
            size_t idx = hash<K>{}(curr->key) % new_capacity;
            curr->next = new_table[idx];
            new_table[idx] = curr;
            curr = next;
        }
    }

    table.swap(new_table);
    capacity = new_capacity;
}

// insert / update
template <typename K, typename V>
void HashMap<K, V>::insert(const K& key, const V& value) {
    resizeIfNeeded();
    size_t idx = hashKey(key);

    Entry* curr = table[idx];
    while (curr) {
        if (curr->key == key) {
            curr->value = value; // update
            return;
        }
        curr = curr->next;
    }

    Entry* e = new Entry(key, value);
    e->next = table[idx];
    table[idx] = e;
    ++count;
}

// get (optional)
template <typename K, typename V>
optional<V> HashMap<K, V>::get(const K& key) const {
    size_t idx = hashKey(key);
    Entry* curr = table[idx];
    while (curr) {
        if (curr->key == key) return curr->value;
        curr = curr->next;
    }
    return nullopt;
}

// erase
template <typename K, typename V>
bool HashMap<K, V>::erase(const K& key) {
    size_t idx = hashKey(key);
    Entry* curr = table[idx];
    Entry* prev = nullptr;

    while (curr) {
        if (curr->key == key) {
            if (prev) prev->next = curr->next;
            else table[idx] = curr->next;
            delete curr;
            --count;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

template <typename K, typename V>
bool HashMap<K, V>::contains(const K& key) const {
    return get(key).has_value();
}

template class HashMap<string, int>;
template class HashMap<int, string>;
template class HashMap<int, int>;

// prefer void* to avoid including LRU in here:
template class HashMap<int, void*>;
template class HashMap<string, void*>;
