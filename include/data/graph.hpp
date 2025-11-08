#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

class Graph {
public:
    void addNode(const string &key);

    void addEdge(const string &from, const string &to);    //add directed edge

    void removeNode(const string &key);

    void removeDependencies(const string &key);

    vector<string> getDependents(const string &key) const;

    vector<string> getAllReachable(const string &key) const;

    // Check if a node exists
    bool hasNode(const string &key) const;

    // Clear the entire graph
    void clear();

private:
    unordered_map<string, unordered_set<string>> adjList;

    void dfs(const string &node,
            unordered_set<string> &visited,
            vector<std::string> &result) const;
};

#endif
