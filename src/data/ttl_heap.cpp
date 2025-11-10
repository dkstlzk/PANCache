#include "data/ttl_heap.hpp"
#include <algorithm>
using namespace std;

template <typename Key, typename Value>
void TTLHeap<Key, Value>::insert(const Key& key, const Value& value, int ttl_seconds) {
    auto now= chrono::steady_clock::now();
    auto expiry= now+ chrono::seconds(ttl_seconds);

    map_[key]= {value, expiry};
    heap_.push_back({key, expiry});
    heapifyUp(heap_.size()-1);
}

template <typename Key, typename Value>
bool TTLHeap<Key, Value>::get(const Key& key, Value& value){

    removeExpired();
    auto it= map_.find(key);
    if (it!=map_.end()) {
        value= it->second.first;
        return true;
    }
    return false;
}

template <typename Key, typename Value>
vector<Key> TTLHeap<Key, Value>::removeExpired() {
    auto now = chrono::steady_clock::now();
    vector<Key> removed;

    while (!heap_.empty() && heap_.front().expiry <= now) {
        Key key = heap_.front().key;

        swap(heap_.front(), heap_.back());
        heap_.pop_back();
        if (!heap_.empty()) heapifyDown(0);

        auto it = map_.find(key);
        if (it != map_.end() && it->second.second <= now) {
            removed.push_back(key);
            map_.erase(it);
        }

    }

    return removed;
}

template <typename Key, typename Value>
size_t TTLHeap<Key, Value>::size() const {
    return map_.size();
}

template <typename Key, typename Value>
void TTLHeap<Key, Value>::heapifyUp(size_t index) {
    while (index>0) {
        size_t parent= (index-1)/2;
        if (heap_[parent]<heap_[index]) break; 
        swapNodes(parent, index);
        index= parent;
    }
}

template <typename Key, typename Value>
void TTLHeap<Key, Value>::heapifyDown(size_t index) {

    size_t n=heap_.size();
    while (true) {
        size_t left= 2*index + 1;
        size_t right= 2*index + 2;
        size_t smallest= index;

        if (left<n && heap_[left]<heap_[smallest]) smallest=left;
        if (right<n && heap_[right]<heap_[smallest]) smallest=right;

        if (smallest==index) break;
        swapNodes(index, smallest);
        index=smallest;
    }

}

template <typename Key, typename Value>
void TTLHeap<Key, Value>::swapNodes(size_t i, size_t j) {
    swap(heap_[i], heap_[j]);
}


template class TTLHeap<string, string>;
template class TTLHeap<int, string>;
template class TTLHeap<string, int>;
template class TTLHeap<int, int>;
