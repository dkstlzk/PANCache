#include "depend/graph.hpp"
#include <iostream>
using namespace std;

int main() {
    cout << "🔗 Testing Graph Dependencies...\n";

    Graph g;
    g.addNode("A");
    g.addNode("B");
    g.addNode("C");

    g.addDependency("A", "B");
    g.addDependency("B", "C");
    g.addDependency("A", "C");

    cout << "Has node A? " << g.hasNode("A") << "\n";

    cout << "Direct dependents of A: ";
    for (auto &d : g.getDependents("A"))
        cout << d << " ";
    cout << "\n";

    cout << "Recursive dependents of A: ";
    for (auto &d : g.getAllDependentsRecursive("A"))
        cout << d << " ";
    cout << "\n";

    g.removeNode("B");
    cout << "After removing B, has B? " << g.hasNode("B") << "\n";

    cout << "✅ Graph test completed successfully.\n";
    return 0;
}
