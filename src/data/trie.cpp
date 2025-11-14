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
