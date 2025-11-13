#include "data/trie.hpp"
#include <iostream>
#include <cassert>
using namespace std;

int main() {
    cout << "Running Trie tests...\n";

    Trie trie;

    trie.insert("apple");
    trie.insert("app");
    trie.insert("apply");

    assert(trie.search("apple"));
    assert(trie.search("app"));
    assert(!trie.search("ap"));   // prefix but not a word
    assert(!trie.search("banana"));

    cout << "Insert + search tests passed.\n";

    auto list1= trie.getWordsWithPrefix("app");
    assert(list1.size()==3);

    auto list2= trie.getWordsWithPrefix("appl");
    assert(list2.size()== 2);

    auto list3= trie.getWordsWithPrefix("zzz");
    assert(list3.empty());

    cout << "✔ Prefix tests passed.\n";

    // test duplicates
    trie.insert("apple");
    trie.insert("apple");

    auto list4 = trie.getWordsWithPrefix("apple");
    assert(list4.size()==1);

    cout << "✔ Duplicate insert test passed.\n";

    // edge cases
    trie.insert("");
    assert(!trie.search(""));     // empty word ignored
    auto list5= trie.getWordsWithPrefix("");
    assert(list5.size()>=3);     // returns all words

    cout << "Edge case tests passed.\n";

    cout << "All Trie tests passed successfully!\n";
    return 0;
}
