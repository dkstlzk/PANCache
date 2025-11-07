#include "data/lru.hpp"
#include <iostream>

int main() {
    std::cout << "Testing LRUCache + HashMap integration...\n";

    // must specify template types (since LRUCache is a template)
    LRUCache<int, int> cache(3);

    cache.put(1, 10);
    cache.put(2, 20);
    cache.put(3, 30);
    cache.display();

    std::cout << "Get(2): " << cache.get(2) << "\n";
    cache.display();

    cache.put(4, 40); // should evict key 1
    cache.display();

    std::cout << "Get(1): " << cache.get(1) << " (should be 0 if evicted)\n";
    std::cout << "Integration test finished.\n";

    return 0;
}
