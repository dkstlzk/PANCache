#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <vector>
#include <string>
#include <functional>
class BloomFilter {
private:
    int size;                     
    int hashCount;                
    std::vector<bool> bits;       

    int hash(const std::string &key, int seed) const{
        std::hash<std::string> hasher;
        return static_cast<int>(hasher(key + std::to_string(seed)) % size);
    }

public:
    BloomFilter(int m = 5000, int k = 3);

    void insert(const std::string &key);
    bool possiblyExists(const std::string &key) const;
    void clear();
};

#endif
