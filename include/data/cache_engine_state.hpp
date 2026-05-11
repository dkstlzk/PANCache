#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

struct CacheEntryState {
    std::string key;
    std::string value;
    int ttl = -1;
    std::string status = "Synced";
};

struct CacheLinkState {
    std::string parent;
    std::string child;
};

struct CacheEngineState {
    std::vector<CacheEntryState> entries;
    std::vector<CacheLinkState> links;
    std::vector<std::string> skiplist;
    std::vector<std::pair<std::string,int>> topk;
    std::vector<std::string> lru;  
    std::unordered_map<std::string,long long> ttl_expiry;
    std::vector<std::string> trie_matches;
    std::vector<std::vector<std::string>> hashmap_buckets;
};


#include <sstream>
#include <iomanip>

inline std::string json_escape(const std::string &s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '\"': out += "\\\""; break;
            default:
                if (c < 0x20) {
                    std::ostringstream oss;
                    oss << "\\u" << std::hex
                        << std::setw(4) << std::setfill('0')
                        << (int)c;
                    out += oss.str();
                } else out.push_back((char)c);
        }
    }
    return out;
}

inline std::string to_json(const CacheEngineState &st) {
    std::ostringstream ss;

    ss << "{";

    // entries
    ss << "\"entries\":[";
    bool fe = true;
    for (auto &e : st.entries) {
        if (!fe) ss << ",";
        fe = false;
        ss << "{"
           << "\"key\":\""   << json_escape(e.key)   << "\","
           << "\"value\":\"" << json_escape(e.value) << "\","
           << "\"ttl\":" << e.ttl << ","
           << "\"status\":\"" << json_escape(e.status) << "\""
           << "}";
    }
    ss << "],";

    // links
    ss << "\"links\":[";
    bool fl = true;
    for (auto &l : st.links) {
        if (!fl) ss << ",";
        fl = false;
        ss << "{"
           << "\"parent\":\"" << json_escape(l.parent) << "\","
           << "\"child\":\""  << json_escape(l.child)  << "\""
           << "}";
    }
    ss << "],";

    // skiplist
    ss << "\"skiplist\":[";
    bool fs = true;
    for (auto &k : st.skiplist) {
        if (!fs) ss << ",";
        fs = false;
        ss << "\"" << json_escape(k) << "\"";
    }
    ss << "],";

    // topk
    ss << "\"topk\":[";
    bool fk = true;
    for (auto &p : st.topk) {
        if (!fk) ss << ",";
        fk = false;
        ss << "{"
           << "\"key\":\"" << json_escape(p.first) << "\","
           << "\"count\":" << p.second
           << "}";
    }
    ss << "],";

    // lru
    ss << "\"lru\":[";
    bool flru = true;
    for (auto &k : st.lru) {
        if (!flru) ss << ",";
        flru = false;
        ss << "\"" << json_escape(k) << "\"";
    }
    ss << "],";

    // ttl expiry
    ss << "\"ttlExpiry\":{";
    bool ft = true;
    for (auto &p : st.ttl_expiry) {
        if (!ft) ss << ",";
        ft = false;
        ss << "\"" << json_escape(p.first) << "\":" << p.second;
    }
    ss << "},";

    // trie matches
    ss << "\"trie_matches\":[";
    bool ftm = true;
    for (auto &k : st.trie_matches) {
        if (!ftm) ss << ",";
        ftm = false;
        ss << "\"" << json_escape(k) << "\"";
    }
    ss << "],";

    // hashmap buckets
    ss << "\"hashmapBuckets\":[";
    bool fb = true;
    for (auto &bucket : st.hashmap_buckets) {
        if (!fb) ss << ",";
        fb = false;
        ss << "[";
        bool fbe = true;
        for (auto &cell : bucket) {
            if (!fbe) ss << ",";
            fbe = false;
            ss << "\"" << json_escape(cell) << "\"";
        }
        ss << "]";
    }
    ss << "]";

    ss << "}";

    return ss.str();
}
