#include "data/cache_engine.hpp"
#include <iostream>
#include <cassert>
#include <thread>
using namespace std;

int main(){

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
    return 0;
}
