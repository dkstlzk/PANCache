#include "../../include/data/graph.hpp"
#include<stack>
using namespace std;

void Graph::addNode(const string &key){
    if(adjList.find(key) ==adjList.end()){
        adjList[key] = {};
    }
}

void Graph::addEdge(const string &from, const string &to){
    addNode(from);
    addNode(to);
    adjList[from].insert(to);
}

void Graph::removeNode(const string &key){
    adjList.erase(key);

    for(auto &pair : adjList){
        pair.second.erase(key);
    }
}

void Graph::removeDependencies(const string &key){
    if(adjList.find(key) != adjList.end()){
        adjList[key].clear();
    }
}

vector<string> Graph::getDependents(const string &key) const{
    vector<string> dependents;
    for(const auto &pair : adjList){
        if(pair.second.find(key) !=pair.second.end()){
            dependents.push_back(pair.first);
        }
    }
    return dependents;
}

void Graph::dfs(const string &node,
                unordered_set<string> &visited,
                vector<string> &result) const{
    if(visited.find(node) !=visited.end()) return;
    visited.insert(node);
    result.push_back(node);

    auto it =adjList.find(node);
    if(it != adjList.end()){
        for(const auto &neighbor : it->second){
            dfs(neighbor, visited, result);
        }
    }
}

vector<string> Graph::getAllReachable(const string &key) const{
    vector<string> result;
    unordered_set<string> visited;
    dfs(key, visited, result);
    return result;
}

bool Graph::hasNode(const string &key) const{
    return adjList.find(key) !=adjList.end();
}

void Graph::clear(){
    adjList.clear();
}
