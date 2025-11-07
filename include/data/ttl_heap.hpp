#pragma once
#include <unordered_map>
#include <vector>
#include <chrono>
#include <string>
#include <utility>
using namespace std;

// TTLHeap: Time-To-Live Expiration Min-Heap
template <typename Key, typename Value>
class TTLHeap{
public:
    TTLHeap()= default;

    void insert(const Key& key, const Value& value, int ttl_seconds);

    // get value if it exists and hasn't expired
    bool get(const Key& key, Value& value);

    void removeExpired();

    size_t size() const;

private:
    using TimePoint= chrono::steady_clock::time_point;

    struct HeapNode{
        Key key;
        TimePoint expiry;
        bool operator<(const HeapNode& other) const{
            return expiry>other.expiry; // min-heap comparator
        }
    };

    unordered_map<Key, pair<Value, TimePoint>> map_;
    vector<HeapNode> heap_;

    void heapifyUp(size_t index);
    void heapifyDown(size_t index);
    void swapNodes(size_t i, size_t j);
};
