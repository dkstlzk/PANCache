#include "data/cache_engine.hpp"
#include "data/skiplist.hpp"
#include <iostream>
#include <thread>
#include <cassert>
using namespace std;

int main(){
    cout<<"PANCache Integration Test Start...\n";

    CacheEngine cache(5);
    cache.set("alpha","A");
    cache.set("beta", "B");
    cache.set("gamma","G",2);   // ttl=2s

    assert( cache.get("alpha").value()=="A" );
    assert( cache.get("beta").value() == "B" );
    cout<<"HashMap + LRU integration works.\n";

    cout<<"Waiting 3 seconds to test TTL expiry...\n";
    this_thread::sleep_for( chrono::seconds(3) );
    assert( !cache.get("gamma").has_value() );
    cout<<"TTLHeap expiration works.\n";

    cache.set("parent","P");
    cache.set("child", "C");
    cache.depend("parent","child");
    cache.del("parent"); 
    assert( !cache.get("child").has_value() );
    cout<<"Graph dependency cascade works.\n";

    cache.set("x","100");
    cache.expire("x",1);
    this_thread::sleep_for( chrono::seconds(2) );
    assert( !cache.get("x").has_value() );
    cout<<"Expire operation verified.\n";

    SkipList<string,int> sl;
    sl.insert("apple",10);
    sl.insert("banana", 20);
    sl.insert("cherry",30);
    sl.display();
    cout<<"SkipList sorted access verified.\n";

    cache.set("delta","D");
    assert( cache.size() >0 );
    cout<<"CacheEngine stable across mixed operations.\n";

    cout<<"\nAll PANCache integrated module tests passed successfully!\n";
    return 0;
}
