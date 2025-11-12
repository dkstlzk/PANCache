// src/main.cpp
#include "cli/command_parser.hpp"
#include "data/cache_engine.hpp"
#include "utils/logger.hpp"

#include <iostream>
#include <string>

using namespace std;
using namespace PANCache::Utils;

int main() {
    // Initialize logger early so other modules can use it
    auto &log = Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);

    log.info("PANCache Core starting up...");
    log.info("Use the following commands to test modules:");
    log.info("  make test_hashmap     → Test HashMap module");
    log.info("  make test_lru         → Test LRUCache module");
    log.info("  make test_ttl         → Test TTLHeap module");
    log.info("  make test_integration → Run full integration test");
    cout << endl;

    // Start CLI (original behavior)
    CacheEngine cache;
    CommandParser cli(cache);

    cout << "PANCache CLI — type HELP for commands.\n";

    string input;
    while (true) {
        cout << "> ";
        if (!std::getline(cin, input)) {
            // EOF or error — exit gracefully
            log.info("Input stream closed, shutting down.");
            break;
        }
        if (!input.empty()) {
            log.debug(string("CLI input: ") + input);
            cli.handleCommand(input);
        }
    }

    log.info("PANCache Core shutting down.");
    return 0;
}
