#include "data/ttl_heap.hpp"
#include <algorithm>
using namespace std;

template <typename Key, typename Value>
void TTLHeap<Key, Value>::insert(const Key& key, const Value& value, int ttl_seconds) {
    erase(key);
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
        if (heap_[parent] < heap_[index]) break;
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

template <typename Key, typename Value>
bool TTLHeap<Key, Value>::erase(const Key& key) {
    auto it = map_.find(key);
    if (it == map_.end()) return false;

    map_.erase(it);
    heap_.erase(remove_if(heap_.begin(), heap_.end(),
        [&](const HeapNode& node) { return node.key == key; }),
        heap_.end());

    for (int i = static_cast<int>(heap_.size()) / 2 - 1; i >= 0; --i)
        heapifyDown(static_cast<size_t>(i));

    return true;
}

template <typename Key, typename Value>
bool TTLHeap<Key, Value>::getExpiry(const Key& key, TimePoint& expiry) const {
    auto it = map_.find(key);
    if (it == map_.end()) return false;
    expiry = it->second.second;
    return true;
}

template <typename Key, typename Value>
unordered_map<Key, long long> TTLHeap<Key, Value>::getExpiryEpochMs() const {
    unordered_map<Key, long long> out;
    out.reserve(map_.size());

    auto now_steady = chrono::steady_clock::now();
    auto now_wall = chrono::system_clock::now();

    for (const auto& kv : map_) {
        const auto& expiry = kv.second.second;
        auto remaining = expiry - now_steady;
        auto expiry_wall = now_wall + remaining;
        long long epoch_ms = chrono::duration_cast<chrono::milliseconds>(
            expiry_wall.time_since_epoch()).count();
        out[kv.first] = epoch_ms;
    }

    return out;
}

template <typename Key, typename Value>
unordered_map<Key, int> TTLHeap<Key, Value>::getRemainingSeconds() const {
    unordered_map<Key, int> out;
    out.reserve(map_.size());

    auto now = chrono::steady_clock::now();
    for (const auto& kv : map_) {
        auto remaining = chrono::duration_cast<chrono::seconds>(
            kv.second.second - now).count();
        if (remaining < 0) remaining = 0;
        out[kv.first] = static_cast<int>(remaining);
    }

    return out;
}

template <typename Key, typename Value>
void TTLHeap<Key, Value>::clear() {
    map_.clear();
    heap_.clear();
}


template class TTLHeap<string, string>;
template class TTLHeap<int, string>;
template class TTLHeap<string, int>;
template class TTLHeap<int, int>;
