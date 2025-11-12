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

    assert(g.hasNode("A"));
    assert(g.hasNode("E"));

    auto deps = g.getDependents("A");
    assert(deps.size() == 2);

    auto allDeps = g.getAllDependentsRecursive("A");
    // Expecting {B, C, D, E}
    assert(allDeps.size() == 4);

    g.removeDependencies("A");
    assert(g.getDependents("A").empty());

    g.removeNode("B");
    assert(!g.hasNode("B"));

    g.clear();
    assert(!g.hasNode("A"));

    cout << "✅ Graph dependency tests passed successfully!\n";
    return 0;
}
