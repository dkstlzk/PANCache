#include "data/lru.hpp"
#include <iostream>

int main() {
    LRUCache<int, int> cache(3);

    cache.put(1, 10);
    cache.put(2, 20);
    cache.put(3, 30);
    cache.display();

    std::cout << "Access 2: " << cache.get(2) << std::endl;
    cache.display();

    cache.put(4, 40); 
    cache.display();

    std::cout << "Access 1 (evicted): " << cache.get(1) << std::endl;
    return 0;
}
