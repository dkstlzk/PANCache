#include "depend/graph.hpp"
using namespace PANCache::Depend;
using namespace std;

void Graph::addNode(const string &key) {
    if (!adjList.count(key))
        adjList[key] = {};
}

void Graph::addDependency(const string &from, const string &to) {
    addNode(from);
    addNode(to);
    adjList[from].insert(to);
}

void Graph::removeNode(const string &key) {
    adjList.erase(key);
    for (auto& p : adjList)
        p.second.erase(key);
}

void Graph::removeDependencies(const string &key) {
    if (adjList.count(key))
        adjList[key].clear();
}

bool Graph::hasNode(const string &key) const {
    return adjList.count(key);
}

vector<string> Graph::getDependents(const string &key) const {
    vector<string> out;
    if (adjList.count(key))
        out.assign(adjList.at(key).begin(), adjList.at(key).end());
    return out;
}

void Graph::dfs(const string &node,
                unordered_set<string> &visited,
                vector<string> &result) const
{
    if (visited.count(node)) return;
    visited.insert(node);

    if (adjList.count(node)) {
        for (auto& dep : adjList.at(node))
            dfs(dep, visited, result);
    }
    result.push_back(node);
}

vector<string> Graph::getAllDependentsRecursive(const string &key) const {
    unordered_set<string> visited;
    vector<string> result;
    dfs(key, visited, result);
    result.erase(result.begin());  // remove self
    return result;
}

void Graph::clear() {
    adjList.clear();
}