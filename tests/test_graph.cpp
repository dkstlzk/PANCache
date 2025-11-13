#include "depend/graph.hpp"
#include "utils/logger.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <algorithm>

using namespace PANCache::Utils;
using namespace PANCache::Depend;
using namespace std;

int main() {
    auto& log = Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);

    cout << "Testing Graph Dependencies...\n";
    log.info("Starting Graph Tests...");

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

    cout << " Graph dependency tests passed successfully!\n";

    log.info("Testing linear chain dependencies...");
    Graph g1;

    g1.addDependency("A", "B");
    g1.addDependency("B", "C");
    g1.addDependency("C", "D");
    g1.addDependency("D", "E");

    auto all1 = g1.getAllDependentsRecursive("A");
    assert(all1.size() == 4);
    log.info("Linear chain test passed.");

    log.info("Testing independent subgraphs...");

    Graph g2;
    g2.addDependency("X", "Y");
    g2.addDependency("P", "Q");

    assert(g2.hasNode("X"));
    assert(g2.hasNode("P"));

    assert(g2.getDependents("X").size() == 1);
    assert(g2.getDependents("P").size() == 1);

    log.info("Independent subgraph test passed.");

    log.info("Testing cyclic graph safety...");

    Graph g3;
    g3.addDependency("A", "B");
    g3.addDependency("B", "C");
    g3.addDependency("C", "A"); // cycle

    auto cyc = g3.getAllDependentsRecursive("A");
    assert(cyc.size() == 3);

    log.info("Cycle test passed (DFS stopped correctly).");


    log.info("Testing removeNode cleanup...");

    Graph g4;
    g4.addDependency("A", "B");
    g4.addDependency("B", "C");
    g4.addDependency("C", "D");

    g4.removeNode("C");

    assert(!g4.hasNode("C"));
    // B was pointing to C → now removed
    assert(g4.getDependents("B").empty());

    log.info("removeNode test passed.");


    log.info("Testing removeDependencies...");

    Graph g5;

    g5.addDependency("A", "B");
    g5.addDependency("A", "C");
    g5.addDependency("A", "D");

    g5.removeDependencies("A");

    assert(g5.getDependents("A").empty());
    assert(g5.hasNode("A"));

    log.info("removeDependencies test passed.");


    log.info("Running graph stress test (1000 nodes)...");

    Graph g6;

    for (int i = 1; i <= 1000; i++)
        g6.addNode("N" + to_string(i));

    for (int i = 1; i <= 999; i++)
        g6.addDependency("N" + to_string(i), "N" + to_string(i+1));

    auto big = g6.getAllDependentsRecursive("N1");
    assert(big.size() == 999);

    log.info("Stress test passed."); 


    log.info("Testing isolated nodes...");

    Graph g7;

    g7.addNode("A");
    g7.addNode("B");
    g7.addNode("C");

    assert(g7.getDependents("A").empty());
    assert(g7.getDependents("B").empty());
    assert(g7.getDependents("C").empty());

    log.info("Isolated nodes test passed.");


    log.info("Testing non-existing key behavior...");

    Graph g8;
    g8.addDependency("A", "B");

    assert(g8.getDependents("Z").empty());
    assert(!g8.hasNode("Z"));

    auto rec = g8.getAllDependentsRecursive("Z");
    assert(rec.size() == 0);

    log.info("Non-existing key tests passed.");


    log.info("Testing duplicate insert safety...");

    Graph g9;

    g9.addDependency("A", "B");
    g9.addDependency("A", "B"); // duplicate

    auto ds = g9.getDependents("A");
    assert(ds.size() == 1);

    log.info("Duplicate edge test passed.");


    log.info("Testing complex mixed graph...");

    Graph g10;

    g10.addDependency("Root1", "A");
    g10.addDependency("Root1", "B");

    g10.addDependency("Root2", "C");
    g10.addDependency("Root2", "D");

    g10.addDependency("A", "X");
    g10.addDependency("B", "Y");
    g10.addDependency("C", "Z");

    auto X = g10.getAllDependentsRecursive("Root1");
    auto Y = g10.getAllDependentsRecursive("Root2");

    assert(X.size() == 4);
    assert(Y.size() == 3);

    log.info("Complex mixed graph test passed.");

    cout << "All Graph tests (extended suite) passed successfully!\n";
    log.info("All Graph tests completed successfully.");


    return 0;
} 
