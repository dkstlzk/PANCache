#pragma once
#include <string>
#include <vector>
using namespace std;

class Trie {
private:
    struct Node {
        bool isEnd;
        Node* children[26];
        Node() : isEnd(false) {
            for (int i=0; i<26; i++) children[i]= nullptr;
        }
    };

    Node* root;

    void collect(Node* node, string prefix, vector<string>& result) const;

    // remove support
    bool removeHelper(Node* node, const string& word, int depth);

public:
    Trie() { root = new Node(); }

    void insert(const string& word);
    bool search(const string& word) const;

    // prefix search
    vector<string> startsWith(const string& prefix) const;
    vector<string> getWordsWithPrefix(const string& prefix) const;

    // remove key
    void remove(const string& word);
};
