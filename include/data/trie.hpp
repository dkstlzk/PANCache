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
            for (int i= 0; i< 26; i++) children[i]= nullptr;
        }
    };

    Node* root;

    void collect(Node* node, string prefix, vector<string>& result) const {
        if (!node) return;

        if (node->isEnd)
            result.push_back(prefix);

        for (int i=0; i<26; i++) {
            if (node->children[i])
                collect(node->children[i], prefix + char('a' + i), result);
        }
    }

public:
    Trie() { root= new Node(); }

    void insert(const string& word);

    bool search(const string& word) const;

    bool startsWith(const string& prefix) const;

    vector<string> getWordsWithPrefix(const string& prefix) const;
};

