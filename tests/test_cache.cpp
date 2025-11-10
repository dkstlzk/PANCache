#include "data/cache_engine.hpp"
#include <iostream>
#include <cassert>
#include <thread>

int main() {

    CacheEngine cache(5);

    cache.set("a", "apple", 2);
    cache.set("b", "banana");

    auto va = cache.get("a");
    assert(va.has_value() && va.value() == "apple");

    cache.set("parent", "P");
    cache.set("child", "C");
    cache.depend("parent", "child");
    cache.del("parent");
    assert(!cache.get("child").has_value());

    cache.set("x", "1", 1);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    assert(!cache.get("x").has_value());

    std::cout << "CacheEngine tests passed!" << std::endl;
    return 0;
    
}
