#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

using namespace std;

class Graph {
public:
    void addNode(const string &key);
    void addDependency(const string &from, const string &to);

    vector<string> getDependents(const string &key) const;
    vector<string> getAllDependentsRecursive(const string &key) const;

    bool hasNode(const string &key) const;
    void removeNode(const string &key);

private:
    unordered_map<string, unordered_set<string>> adj_;
    void dfs(const string &node,
             unordered_set<string> &visited,
             vector<string> &result) const;
};
