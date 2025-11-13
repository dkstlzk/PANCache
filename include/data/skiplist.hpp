#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <optional>

using namespace std;

template <typename K, typename V>
class SkipList {
private:
    struct Node {
        K key;
        V value;
        vector<Node*> forward;
        Node(K k, V v, int level): key(k), value(v), forward(level+1, nullptr) {}
    };

    int maxLevel;
    float probability;
    int currentLevel;
    Node* header;

    int randomLevel() const {
        int lvl=0;
        while (((float)rand()/RAND_MAX)<probability && lvl<maxLevel)
            lvl++;
        return lvl;
    }

public:
    SkipList(int maxL=8, float prob=0.5f): maxLevel(maxL), probability(prob), currentLevel(0) {
        srand((unsigned)time(nullptr));
        header= new Node(K(), V(), maxLevel);
    }

    ~SkipList() {
        Node* curr= header->forward[0];
        while (curr) {
            Node* next= curr->forward[0];
            delete curr;
            curr=next;
        }
        delete header;
    }

    void insert(const K& key, const V& value) {
        vector<Node*> update(maxLevel+1, nullptr);
        Node* curr= header;

        for (int i=currentLevel; i>=0; i--) {
            while (curr->forward[i] && curr->forward[i]->key < key)
                curr= curr->forward[i];
            update[i]= curr;
        }

        curr= curr->forward[0];

        if (curr && curr->key==key){
            curr->value= value; 
            return;
        }

        int newLevel= randomLevel();
        if (newLevel>currentLevel) {
            for (int i= currentLevel+1; i<=newLevel; i++)
                update[i]=header;
            currentLevel=newLevel;
        }

        Node* newNode= new Node(key, value, newLevel);
        for (int i=0; i<=newLevel; i++) {
            newNode->forward[i]= update[i]->forward[i];
            update[i]->forward[i]= newNode;
        }
    }

    optional<V> search(const K& key) const {
        Node* curr= header;
        for (int i= currentLevel; i>=0; i--) {
            while (curr->forward[i] && curr->forward[i]->key < key)
                curr= curr->forward[i];
        }

        curr= curr->forward[0];
        if (curr && curr->key==key)
            return curr->value;

        return nullopt;
    }

    bool erase(const K& key) {
        vector<Node*> update(maxLevel+1, nullptr);
        Node* curr= header;

        for (int i = currentLevel; i>=0; i--) {
            while (curr->forward[i] && curr->forward[i]->key < key)
                curr= curr->forward[i];
            update[i]=curr;
        }

        curr= curr->forward[0];
        if (!curr || curr->key!=key) return false;

        for (int i=0; i<=currentLevel; i++) {
            if (update[i]->forward[i]!=curr) break;
            update[i]->forward[i]= curr->forward[i];
        }
        delete curr;

        while (currentLevel>0 && !header->forward[currentLevel])
            currentLevel--;
        return true;
    }

    void display() const {
        cout << "SkipList contents (level 0): ";
        Node* curr= header->forward[0];
        while (curr) {
            cout << "(" << curr->key << "," << curr->value << ") ";
            curr= curr->forward[0];
        }
        cout << endl;
    }
};

#endif 