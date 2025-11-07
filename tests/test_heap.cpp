#include "data/ttl_heap.hpp"
#include <iostream>
#include <thread>

using namespace PANCache::Data;

int main() {
    TTLHeap<std::string, std::string> ttlHeap;

    ttlHeap.insert("a", "apple", 2);
    ttlHeap.insert("b", "banana", 5);

    std::string val;
    if (ttlHeap.get("a", val)) std::cout << "Got: " << val << "\n";

    std::this_thread::sleep_for(std::chrono::seconds(3));
    if (!ttlHeap.get("a", val)) std::cout << "a expired\n";
    if (ttlHeap.get("b", val)) std::cout << "b still valid: " << val << "\n";

    return 0;
}
