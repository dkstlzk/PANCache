#pragma once
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

class Trie {
private:
    struct Node {
        bool isEnd;
        unordered_map<char, Node*> children;
        Node() : isEnd(false) {}
    };

    Node* root;

    void collect(Node* node, string prefix, vector<string>& result) const;

    void deleteSubtree(Node* node);

    // remove support
    bool removeHelper(Node* node, const string& word, int depth);

public:
    Trie() { root = new Node(); }
    ~Trie();
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    void insert(const string& word);
    bool search(const string& word) const;

    // prefix search
    vector<string> startsWith(const string& prefix) const;
    vector<string> getWordsWithPrefix(const string& prefix) const;

    // remove key
    void remove(const string& word);

    void clear();
};
