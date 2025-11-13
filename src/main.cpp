#include "cli/command_parser.hpp"
#include "data/cache_engine.hpp"
#include "utils/logger.hpp"
#include <iostream>
using namespace std;
using namespace PANCache::Utils;

int main() {
    auto& log = Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);

    CacheEngine cache(50);
    CommandParser cli(cache);

    cout << "PANCache CLI ready.\n";

    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        cli.handleCommand(line);
    }
}