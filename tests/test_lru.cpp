#include "data/lru.hpp"
#include "utils/logger.hpp"
#include <iostream>
#include <cassert>
#include <vector>

using namespace std;
using namespace PANCache::Utils;

int main() { 
    auto &log= Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);

    log.info("Starting LRU Cache tests...");

    LRUCache<int, int> cache(3);

    cache.put(1, 10);
    cache.put(2, 20);
    cache.put(3, 30);
    cache.display();

    if (auto v = cache.get(2); v.has_value())
        cout << "Access 2: " << v.value() << endl;
    else
        cout << "Access 2: [MISS]" << endl;
    cache.display();

    cache.put(4, 40); 
    cache.display();

    if (auto v = cache.get(1); v.has_value())
        cout << "Access 1 (evicted): " << v.value() << endl;
    else
        cout << "Access 1 (evicted): [MISS]" << endl;


    log.info("Testing LRU basic correctness...");
    LRUCache<int,int> c2(2);

    c2.put(1, 100);
    c2.put(2, 200);
    assert(c2.get(1).value()==100);     // 1 becomes MRU
    c2.put(3, 300);               // 2 should be evicted

    assert(!c2.get(2).has_value());      // 2 was LRU => evicted
    assert(c2.get(1).value()==100);
    assert(c2.get(3).value()==300);

    log.info("Testing update (overwrite) behavior...");
    c2.put(1, 999);   // update existing key
    assert(c2.get(1).value()==999);

    log.info("Testing LRU ordering...");
    LRUCache<int,int> c3(3);
    c3.put(10, 1);
    c3.put(20, 2);
    c3.put(30, 3);

    // access 10 => now it should be MRU
    assert(c3.get(10).value()== 1);

    // insert 40 => should evict LRU = 20
    c3.put(40, 4);
    assert(!c3.get(20).has_value());
    assert(c3.get(30).value()==3);
    assert(c3.get(10).value()==1);
    assert(c3.get(40).value()== 4);

    log.info("Testing missing keys...");
    assert(!c3.get(999).has_value());

    log.info("Testing LRU with 1 capacity...");
    LRUCache<int,int> c4(1);
    c4.put(5,55);
    assert(c4.get(5).value()==55);

    c4.put(6,66);        // 5 evicted
    assert(!c4.get(5).has_value());
    assert(c4.get(6).value()==66);

    log.info("Stress testing 500 inserts...");
    LRUCache<int,int> c5(50);
    for(int i=0;i<500;i++) c5.put(i, i*2);
    assert(c5.get(499).value()== 998);  // most recent should remain
    assert(!c5.get(0).has_value());     // old entries should be gone

    log.info("All LRU tests completed.");

    cout << "All LRU tests passed!" << endl;

    return 0;
}
