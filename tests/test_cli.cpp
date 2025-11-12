#include "cli/command_parser.hpp"
#include "data/cache_engine.hpp"
#include <iostream>

int main() {
    CacheEngine engine;
    CommandParser cli(engine);

    std::cout << "✅ CLI Parser test build successful!\n";
    return 0;
}
