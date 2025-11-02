#include "data/hashmap.hpp"
#include <iostream>
#include <cassert>

int main() {
    HashMap<string, int> map;

    // Insert and Get
    map.insert("apple", 10);
    map.insert("banana", 20);
    assert(map.get("apple").value()==10);
    assert(map.get("banana").value()==20);

    // Update
    map.insert("apple", 50);
    assert(map.get("apple").value()==50);

    // Contains
    assert(map.contains("banana"));
    assert(!map.contains("grape"));

    // Erase
    assert(map.erase("banana"));
    assert(!map.erase("banana"));
    assert(!map.contains("banana"));

    // Resize test
    for (int i=0; i<100; i++)
        map.insert("key"+to_string(i), i);

    assert(map.size()>=100);

    cout << "All HashMap tests passed!" << endl;
    return 0;
}
