#include "data/hashmap.hpp"
#include "utils/logger.hpp"
#include <iostream>
#include <cassert>
#include <string>
using namespace std;
using namespace PANCache::Utils;

int main(){
    auto &log= Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);

    log.info("HashMap tests starting...");

    HashMap<string,int> map;

    map.insert("apple",10);
    map.insert("banana", 20);
    assert( map.get("apple").value() ==10 );
    assert( map.get("banana").value()== 20 );

    map.insert("apple", 50);
    assert( map.get("apple").value() == 50 );

    assert( map.contains("banana") );
    assert( !map.contains("grape") );

    assert( map.erase("banana") );
    assert( !map.erase("banana") );
    assert( !map.contains("banana") );

    for(int i=0; i<100; i++)
        map.insert("key"+ to_string(i), i);

    assert(map.size() >=100);

    log.info("Testing get() on missing key...");
    auto missing = map.get("does-not-exist");
    assert(!missing.has_value());

    log.info("Testing overwrite behaviour...");
    map.insert("overwrite", 1);
    map.insert("overwrite", 999);
    assert(map.get("overwrite").value()==999);

    log.info("Testing insert of empty key...");
    map.insert("", 42);
    assert(map.contains(""));
    assert(map.get("").value()==42);

    log.info("Testing collision behavior...");

    map.insert("AaAa", 100);
    map.insert("BBBB", 200);
    assert(map.get("AaAa").value()==100);
    assert(map.get("BBBB").value()== 200);

    log.info("Testing erase edge cases...");
    assert(!map.erase("nonexistent-key"));   // erase should fail safely

    log.info("Stress testing 5000 insertions...");
    for(int i=0; i<5000; i++){
        map.insert("bulk-" + to_string(i), i*2);
    }
    assert(map.get("bulk-4999").value()== 9998);

    log.info("Testing contains correctness after big inserts...");
    assert(map.contains("bulk-1234"));
    assert(!map.contains("bulk-X"));

    cout<<"All HashMap tests passed!\n"<<endl; 
    log.info("All HashMap tests passed!");

    return 0;
}
