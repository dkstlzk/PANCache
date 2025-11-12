#pragma once
#include "data/cache_engine.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <sstream>
using namespace std;

/**
 * @brief CommandParser provides a CLI layer over CacheEngine.
 * Supports commands: SET, GET, DEL, LINK, EXPIRE, SIZE, HELP, EXIT
 */
class CommandParser {
public:
    explicit CommandParser(CacheEngine& engine);

    // Execute one command line
    void handleCommand(const string& input);

    // Print help menu
    void printHelp() const;

private:
    CacheEngine& engine_;

    // Helper: split command string into tokens
    vector<string> tokenize(const string& line) const;

    // Command registry: command name → function
    using CommandHandler = function<void(const vector<string>&)>;
    unordered_map<string, CommandHandler> commands_;

    // Register all available commands
    void registerCommands();

    // Individual command handlers
    void cmdSet(const vector<string>& args);
    void cmdGet(const vector<string>& args);
    void cmdDel(const vector<string>& args);
    void cmdLink(const vector<string>& args);
    void cmdExpire(const vector<string>& args);
    void cmdSize(const vector<string>& args);
};
