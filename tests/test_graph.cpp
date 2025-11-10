<<<<<<< HEAD
#include "data/graph.hpp"
#include <iostream>
#include <cassert>

int main() {
    Graph g;

    g.addEdge("A", "B");
    g.addEdge("A", "C");
    g.addEdge("B", "D");
    g.addEdge("C", "E");
=======
#include "depend/graph.hpp"
#include <iostream>
#include <cassert>

using namespace PANCache::Depend;
using namespace std;

int main() {
    cout << "🔗 Testing Graph Dependencies...\n";

    Graph g;
    g.addDependency("A", "B");
    g.addDependency("A", "C");
    g.addDependency("B", "D");
    g.addDependency("C", "E");
>>>>>>> c1224f3 (feat: add Graph Dependency module and Makefile integration)

    assert(g.hasNode("A"));
    assert(g.hasNode("E"));

<<<<<<< HEAD
    auto dependents = g.getDependents("B");
    assert(dependents.size() == 1 && dependents[0] == "A");

    auto reachable = g.getAllReachable("A");
    // A → {B, C, D, E}
    assert(reachable.size() == 5);

    g.removeNode("C");
    assert(!g.hasNode("C"));
=======
    auto deps = g.getDependents("A");
    assert(deps.size() == 2);

    auto allDeps = g.getAllDependentsRecursive("A");
    // Expecting {B, C, D, E}
    assert(allDeps.size() == 4);

    g.removeDependencies("A");
    assert(g.getDependents("A").empty());

    g.removeNode("B");
    assert(!g.hasNode("B"));
>>>>>>> c1224f3 (feat: add Graph Dependency module and Makefile integration)

    g.clear();
    assert(!g.hasNode("A"));

<<<<<<< HEAD
    std::cout << "All Graph tests passed!\n";
=======
    cout << "✅ Graph dependency tests passed successfully!\n";
>>>>>>> c1224f3 (feat: add Graph Dependency module and Makefile integration)
    return 0;
}
