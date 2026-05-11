#pragma once
#include "data/cache_engine.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <sstream>
using namespace std;

class CommandParser {
public:
    explicit CommandParser(CacheEngine& engine);
    string handleCommand(const string& input);
    string helpText() const;

private:
    CacheEngine& engine_;

    using CommandHandler = function<string(const vector<string>&)>;
    unordered_map<string, CommandHandler> commands_;

    void registerCommands();
    vector<string> tokenize(const string& line) const;

    string cmdSet(const vector<string>& args);
    string cmdGet(const vector<string>& args);
    string cmdDel(const vector<string>& args);
    string cmdLink(const vector<string>& args);
    string cmdExpire(const vector<string>& args);
    string cmdSize(const vector<string>& args);
    string cmdSearch(const vector<string>& args);   
    string cmdPrefix(const vector<string>& args);   
    string cmdTopK(const vector<string>& args);
    string cmdClear(const vector<string>& args);
};
