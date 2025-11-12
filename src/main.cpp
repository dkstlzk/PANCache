#include "cli/command_parser.hpp"
#include "data/cache_engine.hpp"
#include <iostream>
#include <string>
using namespace std;

int main() {
    CacheEngine cache;
    CommandParser cli(cache);

    cout << "PANCache CLI — type HELP for commands.\n";

    string input;
    while (true) {
        cout << "> ";
        getline(cin, input);
        if (!input.empty())
            cli.handleCommand(input);
    }

    return 0;
}
