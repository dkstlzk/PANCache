#pragma once
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <algorithm>
using namespace std;

class TopK {
public:
    vector<pair<string,int>> computeTopK(
        const unordered_map<string,int>& freq, int k)
    {
        priority_queue<pair<int,string>,
                       vector<pair<int,string>>,
                       greater<pair<int,string>>> minHeap;

        for (auto& p : freq) {
            minHeap.push({p.second, p.first});
            if ((int)minHeap.size() > k)
                minHeap.pop();
        }

        vector<pair<string,int>> result;
        while (!minHeap.empty()) {
            auto top = minHeap.top(); minHeap.pop();
            result.push_back({top.second, top.first});
        }
        reverse(result.begin(), result.end());
        return result;
    }
};