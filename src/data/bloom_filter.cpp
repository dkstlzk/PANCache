#include "data/bloom_filter.hpp"
using namespace std;

BloomFilter::BloomFilter(int m, int k) {
    size= m;
    hashCount= k;
    bits.resize(size, false);
}

void BloomFilter::insert(const string &key) {
    for(int i= 0; i< hashCount; i++){
        int index= hash(key, i);
        bits[index] = true;
    }
}

bool BloomFilter::possiblyExists(const string &key) const{
    for(int i= 0; i< hashCount; i++){
        int index = hash(key, i);
        if(!bits[index])
            return false;     
    }
    return true;             
}
