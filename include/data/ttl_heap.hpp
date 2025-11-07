#pragma once
#include <unordered_map>
#include <vector>
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

namespace PANCache::Data {

template <typename Key, typename Value>
class TTLHeap {
public:
    TTLHeap() = default;

    // Insert key with value and TTL (in seconds)
    void insert(const Key& key, const Value& value, int ttl_seconds);

    // Get value if it exists and hasn't expired
    bool get(const Key& key, Value& value);

    // Remove expired entries
    void removeExpired();

    // Current size (valid entries)
    size_t size() const;

private:
    using TimePoint = std::chrono::steady_clock::time_point;

    struct HeapNode {
        Key key;
        TimePoint expiry;
        bool operator<(const HeapNode& other) const {
            return expiry > other.expiry; // min-heap
        }
    };

    std::unordered_map<Key, std::pair<Value, TimePoint>> map_;
    std::vector<HeapNode> heap_;

    void heapifyUp(size_t index);
    void heapifyDown(size_t index);
    void swapNodes(size_t i, size_t j);
};

} // namespace PANCache::Data
