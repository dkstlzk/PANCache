#include "data/trie.hpp"
#include <algorithm>

Trie::~Trie() {
    deleteSubtree(root);
    root = nullptr;
}

void Trie::deleteSubtree(Node* node) {
    if (!node) return;
    for (auto& kv : node->children) {
        deleteSubtree(kv.second);
    }
    delete node;
}

void Trie::insert(const string& word) {
    if (word.empty()) return;
    Node* curr = root;
    for (char c : word) {
        auto& child = curr->children[c];
        if (!child)
            child = new Node();
        curr = child;
    }
    curr->isEnd = true;
}

bool Trie::search(const string& word) const {
    if (word.empty()) return false;
    Node* curr = root;
    for (char c : word) {
        auto it = curr->children.find(c);
        if (it == curr->children.end())
            return false;
        curr = it->second;
    }
    return curr->isEnd;
}

vector<string> Trie::startsWith(const string& prefix) const {
    return getWordsWithPrefix(prefix);
}

vector<string> Trie::getWordsWithPrefix(const string& prefix) const {
    Node* curr = root;
    for (char c : prefix) {
        auto it = curr->children.find(c);
        if (it == curr->children.end())
            return {};
        curr = it->second;
    }

    vector<string> result;
    collect(curr, prefix, result);
    return result;
}

void Trie::collect(Node* node, string prefix, vector<string>& result) const {
    if (!node) return;

    if (node->isEnd)
        result.push_back(prefix);

    vector<pair<char, Node*>> ordered(node->children.begin(), node->children.end());
    sort(ordered.begin(), ordered.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    for (const auto& kv : ordered) {
        collect(kv.second, prefix + kv.first, result);
    }
}

bool Trie::removeHelper(Node* node, const string& word, int depth) {
    if (!node) return false;

    if (depth == (int)word.size()) {
        if (!node->isEnd) return false;
        node->isEnd = false;
        return node->children.empty();
    }

    char c = word[depth];
    auto it = node->children.find(c);
    if (it == node->children.end()) return false;

    if (removeHelper(it->second, word, depth + 1)) {
        delete it->second;
        node->children.erase(it);
        return !node->isEnd && node->children.empty();
    }

    return false;
}

void Trie::remove(const string& word) {
    if (word.empty()) return;
    removeHelper(root, word, 0);
}

void Trie::clear() {
    deleteSubtree(root);
    root = new Node();
}
