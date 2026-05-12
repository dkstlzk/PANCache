#pragma once
#include <string>
#include <vector>
#include <unordered_map>
class Trie {
private:
    struct Node {
        bool isEnd;
        std::unordered_map<char, Node*> children;
        Node() : isEnd(false) {}
    };

    Node* root;

    void collect(Node* node, std::string prefix, std::vector<std::string>& result) const;

    void deleteSubtree(Node* node);

    // remove support
    bool removeHelper(Node* node, const std::string& word, int depth);

public:
    Trie() { root = new Node(); }
    ~Trie();
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    void insert(const std::string& word);
    bool search(const std::string& word) const;

    // prefix search
    std::vector<std::string> startsWith(const std::string& prefix) const;
    std::vector<std::string> getWordsWithPrefix(const std::string& prefix) const;

    // remove key
    void remove(const std::string& word);

    void clear();
};
