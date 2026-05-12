#pragma once
#include "data/cache_engine.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <sstream>
class CommandParser {
public:
    explicit CommandParser(CacheEngine& engine);
    std::string handleCommand(const std::string& input);
    std::string helpText() const;

private:
    CacheEngine& engine_;

    using CommandHandler = std::function<std::string(const std::vector<std::string>&)>;
    std::unordered_map<std::string, CommandHandler> commands_;

    void registerCommands();
    std::vector<std::string> tokenize(const std::string& line) const;

    std::string cmdSet(const std::vector<std::string>& args);
    std::string cmdGet(const std::vector<std::string>& args);
    std::string cmdDel(const std::vector<std::string>& args);
    std::string cmdLink(const std::vector<std::string>& args);
    std::string cmdExpire(const std::vector<std::string>& args);
    std::string cmdSize(const std::vector<std::string>& args);
    std::string cmdSearch(const std::vector<std::string>& args);   
    std::string cmdPrefix(const std::vector<std::string>& args);   
    std::string cmdTopK(const std::vector<std::string>& args);
    std::string cmdClear(const std::vector<std::string>& args);
};
