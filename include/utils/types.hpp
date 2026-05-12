#pragma once
#include <string>
struct CacheEntry {
    std::string key;
    std::string value;
    int ttl;
};