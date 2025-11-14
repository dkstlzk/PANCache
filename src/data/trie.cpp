#include "data/trie.hpp"


void Trie::insert(const string& word) {
    Node* curr= root;
    for (char c: word) {
        if (!isalpha(c)) continue;
        c= tolower(c);

        int idx= c-'a';
        if (!curr->children[idx])
            curr->children[idx]= new Node();
        curr= curr->children[idx];
    }
    curr->isEnd= true;
}

bool Trie::search(const string& word) const {
    Node* curr= root;
    for (char c: word) {
        if (!isalpha(c)) continue;
        c= tolower(c);

        int idx = c-'a';
        if (!curr->children[idx])
            return false;

        curr= curr->children[idx];
    }
    return curr->isEnd;
}

vector<string> Trie::startsWith(const string& prefix) const {
    Node* curr = root;

    for (char c: prefix) {
        if (!isalpha(c)) continue;
        c = tolower(c);

        int idx = c - 'a';
        if (!curr->children[idx])
            return {};  

        curr = curr->children[idx];
    }

    vector<string> result;
    collect(curr, prefix, result);
    return result;
}


vector<string> Trie::getWordsWithPrefix(const string& prefix) const {
    Node* curr= root;
    for (char c: prefix) {
        if (!isalpha(c)) continue;
        c= tolower(c);

        int idx= c-'a';
        if (!curr->children[idx])
            return {};  // no results

        curr= curr->children[idx];
    }

    vector<string> result;
    collect(curr, prefix, result);
    return result;
}

void Trie::collect(Node* node, string prefix, vector<string>& result) const {
    if (!node) return;

    if (node->isEnd)
        result.push_back(prefix);

    for (int i = 0; i < 26; i++) {
        if (node->children[i]) {
            char c = 'a' + i;
            collect(node->children[i], prefix + c, result);
        }
    }
}

bool Trie::removeHelper(Node* node, const string& word, int depth) {
    if (!node) return false;

    // If reached end of word
    if (depth == (int)word.size()) {
        if (!node->isEnd) return false;  // word not present
        node->isEnd = false;

        // Check if node has any children
        for (int i = 0; i < 26; i++)
            if (node->children[i]) return false;  // don't delete parent

        return true;  // delete this node
    }

    char c = word[depth];
    if (!isalpha(c)) return false;
    c = tolower(c);
    int idx = c - 'a';

    if (removeHelper(node->children[idx], word, depth + 1)) {
        delete node->children[idx];
        node->children[idx] = nullptr;

        // If current node has no children and isn't end of another word
        if (!node->isEnd) {
            for (int i = 0; i < 26; i++)
                if (node->children[i]) return false;
            return true;
        }
    }
    return false;
}

void Trie::remove(const string& word) {
    removeHelper(root, word, 0);
}
