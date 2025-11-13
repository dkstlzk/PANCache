#include "data/trie.hpp"
#include <iostream>
#include <cassert>
using namespace std;

int main() {
    cout << "🔍 Running Trie tests...\n";

    Trie trie;

    // ---- Test Insert + Search ----
    trie.insert("apple");
    trie.insert("app");
    trie.insert("apply");

    assert(trie.search("apple"));
    assert(trie.search("app"));
    assert(!trie.search("ap"));   // prefix but not a word
    assert(!trie.search("banana"));

    cout << "✔ Insert + search tests passed.\n";

    // ---- Test startsWith ----
    auto list1 = trie.startsWith("app");
    assert(list1.size() == 3);

    auto list2 = trie.startsWith("appl");
    assert(list2.size() == 2);

    auto list3 = trie.startsWith("zzz");
    assert(list3.empty());

    cout << "✔ Prefix tests passed.\n";

    // ---- Test duplicates ----
    trie.insert("apple");
    trie.insert("apple");

    auto list4 = trie.startsWith("apple");
    assert(list4.size() == 1);

    cout << "✔ Duplicate insert test passed.\n";

    // ---- Edge cases ----
    trie.insert("");
    assert(!trie.search(""));     // empty word ignored
    auto list5 = trie.startsWith("");
    assert(list5.size() >= 3);     // returns all words

    cout << "✔ Edge case tests passed.\n";

    cout << "🎉 All Trie tests passed successfully!\n";
    return 0;
}
