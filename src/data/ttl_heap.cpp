#include "data/ttl_heap.hpp"
#include <algorithm>
#include <string>   

namespace PANCache::Data {

template <typename Key, typename Value>
void TTLHeap<Key, Value>::insert(const Key& key, const Value& value, int ttl_seconds) {
    auto now = std::chrono::steady_clock::now();
    TimePoint expiry = now + std::chrono::seconds(ttl_seconds);

    map_[key] = {value, expiry};
    heap_.push_back({key, expiry});
    heapifyUp(heap_.size() - 1);
}

template <typename Key, typename Value>
bool TTLHeap<Key, Value>::get(const Key& key, Value& value) {
    removeExpired();
    auto it = map_.find(key);
    if (it != map_.end()) {
        value = it->second.first;
        return true;
    }
    return false;
}

template <typename Key, typename Value>
void TTLHeap<Key, Value>::removeExpired() {
    auto now = std::chrono::steady_clock::now();
    while (!heap_.empty() && heap_.front().expiry <= now) {
        Key key = heap_.front().key;
        std::swap(heap_.front(), heap_.back());
        heap_.pop_back();
        heapifyDown(0);
        map_.erase(key);
    }
}

template <typename Key, typename Value>
size_t TTLHeap<Key, Value>::size() const {
    return map_.size();
}

// Heap helpers
template <typename Key, typename Value>
void TTLHeap<Key, Value>::heapifyUp(size_t index) {
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap_[parent] < heap_[index]) break;
        swapNodes(parent, index);
        index = parent;
    }
}

template <typename Key, typename Value>
void TTLHeap<Key, Value>::heapifyDown(size_t index) {
    size_t n = heap_.size();
    while (true) {
        size_t left = 2*index + 1;
        size_t right = 2*index + 2;
        size_t smallest = index;

        if (left < n && heap_[left] < heap_[smallest]) smallest = left;
        if (right < n && heap_[right] < heap_[smallest]) smallest = right;

        if (smallest == index) break;
        swapNodes(index, smallest);
        index = smallest;
    }
}

template <typename Key, typename Value>
void TTLHeap<Key, Value>::swapNodes(size_t i, size_t j) {
    std::swap(heap_[i], heap_[j]);
}

} // namespace PANCache::Data

// Explicit template instantiation (for common types, avoid linker errors)
template class PANCache::Data::TTLHeap<std::string, std::string>;
