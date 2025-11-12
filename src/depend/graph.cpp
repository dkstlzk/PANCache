#include "depend/graph.hpp"
#include <algorithm>
#include <functional>

using namespace std;

// Add a node if it doesn't exist
void Graph::addNode(const string &key) {
    if (!hasNode(key)) {
        adj_[key] = {};
    }
}

// Add directed edge: from -> to
void Graph::addDependency(const string &from, const string &to) {
    addNode(from);
    addNode(to);
    adj_[from].insert(to);
}

// Get direct dependents (one level)
vector<string> Graph::getDependents(const string &key) const {
    vector<string> result;
    auto it = adj_.find(key);
    if (it != adj_.end()) {
        result.insert(result.end(), it->second.begin(), it->second.end());
    }
    return result;
}

// Check if node exists
bool Graph::hasNode(const string &key) const {
    return adj_.find(key) != adj_.end();
}

// Remove a node and its references
void Graph::removeNode(const string &key) {
    adj_.erase(key);
    for (auto &pair : adj_) {
        pair.second.erase(key);
    }
}

// Internal DFS traversal helper
void Graph::dfs(const string &node,
                unordered_set<string> &visited,
                vector<string> &result) const {
    if (visited.count(node)) return;
    visited.insert(node);

    auto it = adj_.find(node);
    if (it != adj_.end()) {
        for (const auto &nbr : it->second) {
            dfs(nbr, visited, result);
        }
    }

    result.push_back(node);
}

// Get all recursive dependents from a node
vector<string> Graph::getAllDependentsRecursive(const string &key) const {
    vector<string> result;
    unordered_set<string> visited;

    dfs(key, visited, result);

    // Remove self (first or last, depending on traversal order)
    if (!result.empty() && result.back() == key)
        result.pop_back();

    reverse(result.begin(), result.end());
    return result;
}
