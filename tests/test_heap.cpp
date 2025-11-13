#include "data/ttl_heap.hpp"
#include "utils/logger.hpp"
#include <iostream>
#include <thread>
#include <cassert> 

using namespace std;
using namespace PANCache::Utils;

int main() {
    auto& log= Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);

    log.info("Starting TTLHeap tests...");

    TTLHeap<string, string> ttlHeap;

    ttlHeap.insert("a", "apple", 2);
    ttlHeap.insert("b", "banana", 5);

    string val;
    if (ttlHeap.get("a", val))
        cout << "Got: " << val << "\n";

    this_thread::sleep_for(chrono::seconds(3));

    if (!ttlHeap.get("a", val))
        cout << "a expired\n";
    if (ttlHeap.get("b", val))
        cout << "b still valid: " << val << "\n";

    log.info("Testing immediate non-expiry...");
    TTLHeap<string,string> h1;
    h1.insert("x", "xxx", 3);
    assert(h1.get("x", val) && val == "xxx");

    log.info("Testing immediate expiry after sleep...");
    TTLHeap<string,string> h2;
    h2.insert("k", "keep", 1);
    this_thread::sleep_for(chrono::milliseconds(1200));
    assert(!h2.get("k", val));

    log.info("Testing multiple TTL ordering...");
    TTLHeap<string,string> h3;
    h3.insert("short", "s", 1);
    h3.insert("mid", "m", 3);
    h3.insert("long", "l", 5);

    this_thread::sleep_for(chrono::seconds(2));
    assert(!h3.get("short", val));
    assert(h3.get("mid", val) && val=="m");
    assert(h3.get("long", val) && val=="l");

    log.info("Testing overwrite behavior...");
    TTLHeap<string,string> h4;
    h4.insert("p", "pine", 5);
    h4.insert("p", "peach", 5);  // overwrite

    assert(h4.get("p", val) && val=="peach");

    log.info("Testing reinsert after expiry...");
    TTLHeap<string,string> h5;
    h5.insert("a", "1", 1);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    assert(!h5.get("a", val));  // expired

    h5.insert("a", "2", 3);
    assert(h5.get("a", val) && val=="2");

    log.info("Testing expiration cleanup with many keys...");
    TTLHeap<int,int> h6;
    for (int i=0; i<50; i++) {
        h6.insert(i, i*10, 1);
    }
    this_thread::sleep_for(chrono::seconds(2));

    int out;
    int valid_count=0;
    for (int i=0; i<50; i++) {
        if (h6.get(i, out)) valid_count++;
    }

    assert(valid_count==0);

    log.info("Testing mixed valid + expired scenario...");
    TTLHeap<string,string> h7;
    h7.insert("A", "alpha", 1);
    h7.insert("B", "beta", 3);

    this_thread::sleep_for(std::chrono::seconds(2));
    assert(!h7.get("A", val));  
    assert(h7.get("B", val) && val=="beta");

    log.info("All TTL tests completed successfully!");

    cout << "All TTLHeap tests passed!" << endl;

    return 0;
}
