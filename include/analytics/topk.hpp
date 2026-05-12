#pragma once
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <algorithm>
class TopK {
public:
    std::vector<std::pair<std::string,int>> computeTopK(
        const std::unordered_map<std::string,int>& freq, int k)
    {
        std::priority_queue<std::pair<int,std::string>,
                            std::vector<std::pair<int,std::string>>,
                            std::greater<std::pair<int,std::string>>> minHeap;

        for (const auto& p : freq) {
            minHeap.push({p.second, p.first});
            if ((int)minHeap.size() > k)
                minHeap.pop();
        }

        std::vector<std::pair<std::string,int>> result;
        while (!minHeap.empty()) {
            auto top = minHeap.top(); minHeap.pop();
            result.push_back({top.second, top.first});
        }
        std::reverse(result.begin(), result.end());
        return result;
    }
};