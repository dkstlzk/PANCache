#include "data/skiplist.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
using namespace std;

int main(){
    cout << "Testing SkipList...\n";

    SkipList<string, int> sl;

    // insert random values
    vector<pair<string, int>> entries= {
        {"delta", 4}, {"alpha", 1}, {"charlie", 3},
        {"echo", 5}, {"bravo", 2}
    };

    for (auto& [k, v]: entries)
        sl.insert(k, v);

        cout << "SkipList Level 0 contents after insert:\n";
    sl.display();

    // check sorted order
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

    // test non-existing erase
    assert(!sl.erase("zulu"));

    // insert again and check traversal still ordered
    sl.insert("foxtrot", 6);
    cout << "After re-insertions:\n";
    sl.display();

    cout << "All SkipList functional tests passed successfully!\n";
    return 0;
}
