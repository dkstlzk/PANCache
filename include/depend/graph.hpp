#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace PANCache {
namespace Depend {

class Graph {
private:
    std::unordered_map<std::string, std::unordered_set<std::string>> adjList;

    void dfs(const std::string &node,
             std::unordered_set<std::string> &visited,
             std::vector<std::string> &result) const;

public:
    void addNode(const std::string &key);
    void addDependency(const std::string &from, const std::string &to);
    void removeNode(const std::string &key);
    void removeDependencies(const std::string &key);
    bool hasNode(const std::string &key) const;
    std::vector<std::string> getDependents(const std::string &key) const;
    std::vector<std::string> getAllDependentsRecursive(const std::string &key) const;
    void clear();
};

} // namespace Depend
} // namespace PANCache
