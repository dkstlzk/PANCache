#include "data/lru.hpp"
#include <iostream>
using namespace std;

template <typename K, typename V>
LRUCache<K, V>::LRUCache(int cap)
    : capacity(cap), count(0), head(nullptr), tail(nullptr) {}

template <typename K, typename V>
LRUCache<K, V>::~LRUCache() {
    Node* curr = head;
    while (curr) {
        Node* temp = curr;
        curr = curr->next;
        delete temp;
    }
}

template <typename K, typename V>
void LRUCache<K, V>::removeNode(Node* node) {
    if (!node) return;
    if (node->prev) node->prev->next = node->next;
    else head = node->next;

    if (node->next) node->next->prev = node->prev;
    else tail = node->prev;
}

template <typename K, typename V>
void LRUCache<K, V>::addToFront(Node* node) {
    node->prev = nullptr;
    node->next = head;
    if (head) head->prev = node;
    head = node;
    if (!tail) tail = head;
}

template <typename K, typename V>
void LRUCache<K, V>::moveToFront(Node* node) {
    removeNode(node);
    addToFront(node);
}

template <typename K, typename V>
void LRUCache<K, V>::put(const K& key, const V& value) {
    auto nodeOpt = cacheMap.get(key);
    if (nodeOpt.has_value()) {
        Node* node = static_cast<Node*>(nodeOpt.value());
        node->value = value;
        moveToFront(node);
        return;
    }

    Node* newNode = new Node(key, value);
    cacheMap.insert(key, static_cast<void*>(newNode));
    addToFront(newNode);
    count++;

    if (count > capacity) {
        Node* lru = tail;
        cacheMap.erase(lru->key);
        removeNode(lru);
        delete lru;
        count--;
    }
}

template <typename K, typename V>
V LRUCache<K, V>::get(const K& key) {
    auto nodeOpt = cacheMap.get(key);
    if (!nodeOpt.has_value()) {
        cout << "Key " << key << " not found!\n";
        return V();  // default value
    }
    Node* node = static_cast<Node*>(nodeOpt.value());
    moveToFront(node);
    return node->value;
}

template <typename K, typename V>
bool LRUCache<K, V>::erase(const K& key) {
    auto nodeOpt = cacheMap.get(key);
    if (!nodeOpt.has_value()) return false;

    Node* node = static_cast<Node*>(nodeOpt.value());
    removeNode(node);
    cacheMap.erase(key);
    delete node;
    count--;
    return true;
}

template <typename K, typename V>
void LRUCache<K, V>::display() const {
    Node* curr = head;
    cout << "Cache: ";
    while (curr) {
        cout << "(" << curr->key << ", " << curr->value << ") ";
        curr = curr->next;
    }
    cout << endl;
}

// Explicit template instantiation
template class LRUCache<int, int>;
template class LRUCache<string, int>;
template class LRUCache<string, string>;
