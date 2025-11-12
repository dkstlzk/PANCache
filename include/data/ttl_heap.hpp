#pragma once
#include <unordered_map>
#include <vector>
#include <chrono>
#include <string>
#include <utility>

using namespace std;

template <typename Key, typename Value>
class TTLHeap {
public:
    TTLHeap() = default;

    // Insert key with value and TTL (in seconds)
    void insert(const Key& key, const Value& value, int ttl_seconds);

    // Get value if it exists and hasn't expired
    bool get(const Key& key, Value& value);

    // Remove all expired keys and return their list
    std::vector<Key> removeExpired();   // ✅ fixed spacing + added std::

    // Return the number of items still valid
    size_t size() const;

private:
    using TimePoint = chrono::steady_clock::time_point;

    struct HeapNode {
        Key key;
        TimePoint expiry;

        // Min-heap comparator: earlier expiry has higher priority
        bool operator<(const HeapNode& other) const {
            return expiry > other.expiry;
        }
    };

    unordered_map<Key, pair<Value, TimePoint>> map_;
    vector<HeapNode> heap_;

    void heapifyUp(size_t index);
    void heapifyDown(size_t index);
    void swapNodes(size_t i, size_t j);
};
