#include "data/graph.hpp"
#include <iostream>
#include <cassert>

int main() {
    Graph g;

    g.addEdge("A", "B");
    g.addEdge("A", "C");
    g.addEdge("B", "D");
    g.addEdge("C", "E");

    assert(g.hasNode("A"));
    assert(g.hasNode("E"));

    auto dependents = g.getDependents("B");
    assert(dependents.size() == 1 && dependents[0] == "A");

    auto reachable = g.getAllReachable("A");
    // A → {B, C, D, E}
    assert(reachable.size() == 5);

    g.removeNode("C");
    assert(!g.hasNode("C"));

    g.clear();
    assert(!g.hasNode("A"));

    std::cout << "All Graph tests passed!\n";
    return 0;
}
