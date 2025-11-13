#include "data/cache_engine.hpp"
#include "utils/logger.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <vector>
using namespace std; 
using namespace PANCache::Utils;

int main(){

    auto& log= Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);

    log.info("Starting CacheEngine Test Suite...");

    CacheEngine cache(5);

    cache.set("a","apple",2);
    cache.set("b", "banana");

    auto va= cache.get("a");
    assert(va.has_value() && va.value()=="apple");

    cache.set("parent","P");
    cache.set("child", "C");
    cache.depend("parent","child");
    cache.del("parent");
    assert( !cache.get("child").has_value() );

    cache.set("x","1",1);
    this_thread::sleep_for( chrono::seconds(2) );
    assert( !cache.get("x").has_value() );

    cout<<"CacheEngine tests passed!\n";


    log.info("Testing TTL expiry ordering...");

    CacheEngine c1(10);

    c1.set("A","aaa",1);
    c1.set("B","bbb",3);
    c1.set("C","ccc",5);

    this_thread::sleep_for(chrono::seconds(2));

    assert(!c1.get("A").has_value());
    assert(c1.get("B").has_value());
    assert(c1.get("C").has_value());

    log.info("TTL staggered expiry test passed.");


    log.info("Testing LRU eviction...");

    CacheEngine c2(3);

    c2.set("k1","1");
    c2.set("k2","2");
    c2.set("k3","3");

    c2.get("k1");  // k1 becomes most recently used
    c2.set("k4","4"); // k2 should be evicted (LRU)

    assert(!c2.get("k2").has_value());
    assert(c2.get("k1").has_value());
    assert(c2.get("k3").has_value());
    assert(c2.get("k4").has_value());

    log.info("LRU eviction test passed.");


    log.info("Testing recursive dependency deletion...");

    CacheEngine c3(10);

    c3.set("root","R");
    c3.set("child1","C1");
    c3.set("child2","C2");
    c3.set("grand","G");

    c3.depend("root","child1");
    c3.depend("root","child2");
    c3.depend("child1","grand");

    c3.del("root");

    assert(!c3.get("child1").has_value());
    assert(!c3.get("child2").has_value());
    assert(!c3.get("grand").has_value());

    log.info("Recursive dependency deletion test passed."); 


    log.info("Testing update behavior...");

    CacheEngine c4(5);

    c4.set("A","10",5);
    c4.set("A","20",5);

    assert(c4.get("A").value()=="20");

    log.info("Update behavior test passed."); 


    log.info("Testing safe invalid deletions...");

    CacheEngine c5(5);

    c5.set("x","100");
    c5.del("unknown");  
    assert(!c5.get("unknown").has_value());
    c5.del("x");
    assert(!c5.get("x").has_value());

    assert(!c5.get("x").has_value());

    log.info("Safe deletion test passed.");


    log.info("Running TTL + LRU combined stress test...");

    CacheEngine c6(5);

    c6.set("A","1",1);
    c6.set("B","2",10);
    c6.set("C","3");
    c6.set("D","4");
    c6.set("E","5");

    this_thread::sleep_for(chrono::seconds(2));

    // A must be expired
    assert(!c6.get("A").has_value());

    // filling to capacity again to test LRU
    c6.set("F","6");

    log.info("Stress test passed.");


    log.info("Testing dependency + TTL interaction...");

    CacheEngine c7(10);

    c7.set("A","1");
    c7.set("B","2",1);
    c7.depend("A","B");

    this_thread::sleep_for(chrono::seconds(2));

    // B expires, so removing A must NOT fail
    c7.del("A");
    assert(!c7.get("A").has_value());
    assert(!c7.get("A").has_value());
    assert(!c7.get("B").has_value());

    log.info("Dependency + TTL interaction passed."); 


    log.info("Testing large-scale insertion...");

    CacheEngine c8(2000);

    for(int i=0;i<1500;i++)
        c8.set("key"+to_string(i), "v");

    for(int i=0;i<1000;i++)
        assert(c8.get("key"+to_string(i)).has_value());

    log.info("Large scale insertion tests passed.");


    log.info("Testing depend() auto node creation...");

    CacheEngine c9(10);

    c9.depend("X","Y");  // neither existed
    c9.set("X","10");
    c9.set("Y","5");

    // delete parent should delete child
    c9.del("X");
    assert(!c9.get("Y").has_value());

    log.info("depend() auto node creation passed.");


    log.info("Testing auto removal on expired access...");

    CacheEngine c10(5);

    c10.set("temp","hello",1);
    this_thread::sleep_for(chrono::seconds(2));

    assert(!c10.get("temp").has_value());
    // after get, it must be physically removed
    c10.del("temp");
    assert(!c10.get("temp").has_value());

    log.info("Expired-key auto-clean test passed.");


    cout << "All CacheEngine tests passed successfully!" << endl;
    log.info("All CacheEngine tests passed.");

    return 0;
}
