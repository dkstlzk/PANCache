#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

namespace PANCache {
namespace Depend {

class Graph {
private:
    unordered_map<string, unordered_set<string>> adjList;

    void dfs(const string &node,
             unordered_set<string> &visited,
             vector<string> &result) const;

public:
    void addNode(const string &key);
    void addDependency(const string &from, const string &to);
    void removeNode(const string &key);
    void removeDependencies(const string &key);
    bool hasNode(const string &key) const;
    vector<string> getDependents(const string &key) const;
    vector<string> getAllDependentsRecursive(const string &key) const;
    void clear();
};

}
}