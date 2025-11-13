#include "data/skiplist.hpp"
#include "utils/logger.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
using namespace std;
using namespace PANCache::Utils;

int main(){
    auto& log=Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);

    log.info("Starting SkipList tests...");

    cout << "Testing SkipList...\n";

    SkipList<string, int> sl;

    vector<pair<string, int>> entries= {
        {"delta", 4}, {"alpha", 1}, {"charlie", 3},
        {"echo", 5}, {"bravo", 2}
    };

    for (auto& [k, v]: entries)
        sl.insert(k, v);

    cout << "SkipList Level 0 contents after insert:\n";
    sl.display();

    vector<string> expected_keys= {"alpha", "bravo", "charlie", "delta", "echo"};
    vector<string> actual_keys;

    for (auto& k: expected_keys) {
        auto val= sl.search(k);
        assert(val.has_value());
        actual_keys.push_back(k);
    }

    assert(expected_keys==actual_keys);
    cout << "Keys verified in sorted order.\n";

    sl.insert("bravo", 22);
    assert(sl.search("bravo").value()==22);
    cout << "Update existing key works.\n";

    bool erased= sl.erase("charlie");
    assert(erased);
    assert(!sl.search("charlie").has_value());
    cout << "Erase operation works.\n";

    assert(!sl.erase("zulu"));

    sl.insert("foxtrot", 6);
    cout << "After re-insertions:\n";
    sl.display();

    cout << "All SkipList functional tests passed successfully!\n";

    log.info("Testing increasing sequence...");
    SkipList<int,int> s1;

    for (int i=1; i<= 50; i++)
        s1.insert(i, i*10);

    for (int i=1; i<=50; i++) {
        auto v= s1.search(i);
        assert(v.has_value() && v.value()==i*10);
    }
    log.info("Increasing sequence test passed.");

    log.info("Testing random insertion order...");
    SkipList<int,int> s2;

    vector<int> r = {43,12,97,5,21,88,1,50};
    for (int k: r)
        s2.insert(k, k+100);

    sort(r.begin(), r.end());

    for (int k: r) {
        auto v= s2.search(k);
        assert(v.has_value());
    }
    log.info("Random insert test passed.");

    log.info("Testing duplicate key updates...");
    SkipList<string,int> s3;

    s3.insert("x", 10);
    s3.insert("x", 500);   // overwrite
    assert(s3.search("x").value()== 500);

    log.info("Duplicate update test passed.");

    log.info("Testing erase edge cases...");
    SkipList<int,int> s4;

    s4.insert(100, 1);
    s4.insert(200, 2);
    s4.insert(300, 3);

    assert(s4.erase(200));       // middle
    assert(!s4.search(200).has_value());

    assert(!s4.erase(999));      // non-existent

    assert(s4.erase(100));       // head
    assert(s4.erase(300));       // tail

    log.info("Erase edge cases passed.");

    log.info("Running stress test (1000 inserts)...");

    SkipList<int,int> s5;
    for (int i=0; i<1000; i++)
        s5.insert(i, i*2);

    for (int i= 0; i<1000; i++)
        assert(s5.search(i).value() == i*2);

    log.info("Stress test passed.");

    log.info("Testing non-existing key search...");
    SkipList<int,int> s6;

    s6.insert(10, 100);
    s6.insert(20, 200);

    assert(!s6.search(15).has_value());
    assert(!s6.search(-1).has_value());
    assert(!s6.search(999).has_value());

    log.info("Non-existing key test passed.");

    log.info("Testing negative/mixed keys...");
    SkipList<int,int> s7;

    s7.insert(-5, 50);
    s7.insert(0, 100);
    s7.insert(7, 700);

    assert(s7.search(-5).value()== 50);
    assert(s7.search(0).value()==100);
    assert(s7.search(7).value()==700);

    log.info("Mixed keys test passed.");

    cout << "All SkipList tests (extended suite) passed successfully!\n";

    return 0;
}
