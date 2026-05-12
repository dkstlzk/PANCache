#pragma once
#include <unordered_map>
#include <vector>
#include <chrono>
#include <string>
#include <utility>
#include <optional>

template <typename Key, typename Value>
class TTLHeap {
public:
    TTLHeap() = default;

    void insert(const Key& key, const Value& value, int ttl_seconds);

    bool get(const Key& key, Value& value);

    std::vector<Key> removeExpired();  

    std::size_t size() const;

private:
    using TimePoint = std::chrono::steady_clock::time_point;

    struct HeapNode {
        Key key;
        TimePoint expiry;

        bool operator<(const HeapNode& other) const {
            return expiry < other.expiry;
        }
    };

    std::unordered_map<Key, std::pair<Value, TimePoint>> map_;
    std::vector<HeapNode> heap_;

    void heapifyUp(std::size_t index);
    void heapifyDown(std::size_t index);
    void swapNodes(std::size_t i, std::size_t j);

public:
    bool erase(const Key& key);
    bool getExpiry(const Key& key, TimePoint& expiry) const;
    std::unordered_map<Key, long long> getExpiryEpochMs() const;
    std::unordered_map<Key, int> getRemainingSeconds() const;
    void clear();
};
