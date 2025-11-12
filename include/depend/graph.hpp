#pragma once
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>

namespace PANCache {
namespace Depend {

class Graph {
private:
    std::unordered_map<std::string, std::vector<std::string>> adj;

public:
    // Add a dependency edge: A depends on B
    void addDependency(const std::string &a, const std::string &b) {
        adj[a].push_back(b);
        if (adj.find(b) == adj.end()) {
            adj[b] = {}; // ensure node exists
        }
    }

    // Remove all dependencies from a node
    void removeDependencies(const std::string &node) {
        adj[node].clear();
    }

    // Remove a node entirely
    void removeNode(const std::string &node) {
        adj.erase(node);
        for (auto &kv : adj) {
            auto &list = kv.second;
            list.erase(std::remove(list.begin(), list.end(), node), list.end());
        }
    }

    // Get direct dependents of a node
    std::vector<std::string> getDependents(const std::string &node) const {
        auto it = adj.find(node);
        if (it != adj.end()) return it->second;
        return {};
    }

    // Check if a node exists
    bool hasNode(const std::string &node) const {
        return adj.find(node) != adj.end();
    }

    // Get all dependents recursively (BFS)
    std::vector<std::string> getAllDependentsRecursive(const std::string &node) const {
        std::vector<std::string> result;
        std::queue<std::string> q;
        std::unordered_set<std::string> visited;

        q.push(node);
        visited.insert(node);

        while (!q.empty()) {
            std::string current = q.front();
            q.pop();
            auto it = adj.find(current);
            if (it == adj.end()) continue;

            for (auto &dep : it->second) {
                if (visited.insert(dep).second) {
                    result.push_back(dep);
                    q.push(dep);
                }
            }
        }
        return result;
    }

    void clear() {
        adj.clear();
    }
};

} // namespace Depend
} // namespace PANCache
