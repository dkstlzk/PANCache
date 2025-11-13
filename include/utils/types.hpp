#pragma once
#include <string>
using namespace std;

struct CacheEntry {
    string key;
    string value;
    int ttl;
};