#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <vector>
#include <string>
#include <functional>
using namespace std;

class BloomFilter {
private:
    int size;                     
    int hashCount;                
    vector<bool> bits;       

    int hash(const string &key, int seed) const{
        std::hash<std::string> hasher;
        return (hasher(key + to_string(seed)) % size);
    }

public:
    BloomFilter(int m = 5000, int k = 3);

    void insert(const string &key);
    bool possiblyExists(const string &key) const;
};

#endif
