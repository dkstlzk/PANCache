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
 * CommandParser provides CLI over CacheEngine.
 */
class CommandParser {
public:
    explicit CommandParser(CacheEngine& engine);
    void handleCommand(const string& input);
    void printHelp() const;

private:
    CacheEngine& engine_;

    using CommandHandler = function<void(const vector<string>&)>;
    unordered_map<string, CommandHandler> commands_;

    void registerCommands();
    vector<string> tokenize(const string& line) const; // ✅ Added

    // Command handlers
    void cmdSet(const vector<string>& args);
    void cmdGet(const vector<string>& args);
    void cmdDel(const vector<string>& args);
    void cmdLink(const vector<string>& args);
    void cmdExpire(const vector<string>& args);
    void cmdSize(const vector<string>& args);
};
