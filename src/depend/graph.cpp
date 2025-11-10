#include "depend/graph.hpp"
using namespace PANCache::Depend;
using namespace std;

// Add a new node if it doesn't exist
void Graph::addNode(const string& key) {
    if (!hasNode(key)) {
        adjList[key] = {};
    }
}

// Add a directed dependency edge: from → to
void Graph::addDependency(const string& from, const string& to) {
    addNode(from);
    addNode(to);
    adjList[from].insert(to);
}

// Remove a node completely (and remove it from others’ dependents)
void Graph::removeNode(const string& key) {
    adjList.erase(key);
    for (auto& [src, deps] : adjList) {
        deps.erase(key);
    }
}

// Remove all outgoing dependencies from a node
void Graph::removeDependencies(const string& key) {
    if (adjList.find(key) != adjList.end()) {
        adjList[key].clear();
    }
}

// Check if a node exists
bool Graph::hasNode(const string& key) const {
    return adjList.find(key) != adjList.end();
}

// Get direct dependents (one level)
vector<string> Graph::getDependents(const string& key) const {
    vector<string> result;
    auto it = adjList.find(key);
    if (it != adjList.end()) {
        result.insert(result.end(), it->second.begin(), it->second.end());
    }
    return result;
}

// Depth-first search to collect all recursive dependents
void Graph::dfs(const string& node,
                unordered_set<string>& visited,
                vector<string>& result) const {
    if (visited.count(node)) return;
    visited.insert(node);
    auto it = adjList.find(node);
    if (it != adjList.end()) {
        for (const auto& dep : it->second) {
            result.push_back(dep);
            dfs(dep, visited, result);
        }
    }
}

// Get all transitive dependents (recursive reachability)
vector<string> Graph::getAllDependentsRecursive(const string& key) const {
    vector<string> result;
    unordered_set<string> visited;
    dfs(key, visited, result);
    return result;
}

// Clear the entire graph
void Graph::clear() {
    adjList.clear();
}
