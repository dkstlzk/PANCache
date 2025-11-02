#include "data/hashmap.hpp"
#include <iostream>
#include "data/lru.hpp"

template <typename K, typename V>
HashMap<K, V>::HashMap(size_t initial_capacity): count(0), capacity(initial_capacity), table(initial_capacity, nullptr){}

template <typename K, typename V>
HashMap<K, V>::~HashMap(){
    clear();
}

template <typename K, typename V>
void HashMap<K, V>::clear(){
    for (auto& head: table){
        Entry* curr= head;
        while (curr){
            Entry* temp= curr;
            curr= curr->next;
            delete temp;
        }
        head=nullptr;
    }
    count=0;
}

template <typename K, typename V>
void HashMap<K, V>::resizeIfNeeded(){
    if ((float)count/capacity <load_factor) return;

    size_t new_capacity= capacity*2;
    vector<Entry*> new_table(new_capacity, nullptr);

    for (auto& head: table){
        Entry* curr= head;
        while (curr){
            Entry* next= curr->next;
            size_t idx= hash<K>{}(curr->key)%new_capacity;
            curr->next= new_table[idx];
            new_table[idx]= curr;
            curr=next;
        }
    }

    table.swap(new_table);
    capacity=new_capacity;
}

template <typename K, typename V>
void HashMap<K, V>::insert(const K& key, const V& value){
    resizeIfNeeded();
    size_t idx= hashKey(key);
    Entry* curr= table[idx];
    while (curr){
        if (curr->key==key){
            curr->value =value;
            return;
        }
        curr= curr->next;
    }
    Entry* newEntry= new Entry(key, value);
    newEntry->next= table[idx];
    table[idx]=newEntry;
    count++;
}

template <typename K, typename V>
optional<V> HashMap<K, V>::get(const K& key) const{
    size_t idx= hashKey(key);
    Entry* curr= table[idx];
    while (curr){
        if (curr->key==key)
            return curr->value;
        curr= curr->next;
    }
    return nullopt;
}

template <typename K, typename V>
bool HashMap<K, V>::erase(const K& key){
    size_t idx= hashKey(key);
    Entry* curr= table[idx];
    Entry* prev= nullptr;

    while (curr){
        if (curr->key==key){
            if (prev) prev->next= curr->next;
            else table[idx]= curr->next;
            delete curr;
            count--;
            return true;
        }
        prev=curr;
        curr=curr->next;
    }
    return false;
}

template <typename K, typename V>
bool HashMap<K, V>::contains(const K& key) const{
    return get(key).has_value();
}

// Explicit template instantiation for common types
template class HashMap<string, int>;
template class HashMap<int, string>;

// Explicit instantiations of template combinations
template class HashMap<int, int>;
template class HashMap<int, LRUCache<int, int>::Node *>;
template class HashMap<std::string, LRUCache<std::string, int>::Node *>;

