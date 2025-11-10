#ifndef DEPEND_GRAPH_HPP
#define DEPEND_GRAPH_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

namespace PANCache {
namespace Depend {

/**
 * @brief Directed dependency graph for managing cache key relationships.
 * 
 * Each node represents a cache key.
 * An edge A → B means: B depends on A.
 * If A is deleted/invalidated, B should also be deleted.
 */
class Graph {
public:
    Graph() = default;

    // --- Core Operations ---
    void addNode(const std::string& key);
    void addDependency(const std::string& from, const std::string& to);
    void removeNode(const std::string& key);
    void removeDependencies(const std::string& key);

    // --- Query Operations ---
    bool hasNode(const std::string& key) const;
    std::vector<std::string> getDependents(const std::string& key) const;
    std::vector<std::string> getAllDependentsRecursive(const std::string& key) const;

    // --- Maintenance ---
    void clear();
    size_t size() const { return adjList.size(); }

private:
    // adjacency list: from -> set of nodes depending on 'from'
    std::unordered_map<std::string, std::unordered_set<std::string>> adjList;

    void dfs(const std::string& node,
             std::unordered_set<std::string>& visited,
             std::vector<std::string>& result) const;
};

} // namespace Depend
} // namespace PANCache

#endif // DEPEND_GRAPH_HPP
