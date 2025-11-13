#include "../include/depend/graph.hpp"
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>

using namespace PANCache::Depend;
using namespace std;

void Graph::addNode(const string &key) {
    if (!adjList.count(key)) adjList[key] = {};
}

void Graph::addDependency(const string &from, const string &to) {
    addNode(from);
    addNode(to);
    adjList[from].insert(to);
}

void Graph::removeNode(const string &key) {
    adjList.erase(key);
    for (auto &pair : adjList)
        pair.second.erase(key);
}

void Graph::removeDependencies(const string &key) {
    adjList[key].clear();
}

bool Graph::hasNode(const string &key) const {
    return adjList.count(key) > 0;
}

vector<string> Graph::getDependents(const string &key) const {
    vector<string> deps;
    auto it = adjList.find(key);
    if (it != adjList.end())
        deps.assign(it->second.begin(), it->second.end());
    return deps;
}

void Graph::dfs(const string &node,
                unordered_set<string> &visited,
                vector<string> &result) const {
    if (visited.count(node)) return;
    visited.insert(node);
    auto it = adjList.find(node);
    if (it != adjList.end()) {
        for (const auto &dep : it->second)
            dfs(dep, visited, result);
    }
    result.push_back(node);
}

vector<string> Graph::getAllDependentsRecursive(const string &key) const {
    unordered_set<string> visited;
    vector<string> result;
    dfs(key, visited, result);
    return result;
}

void Graph::clear() {
    adjList.clear();
}
