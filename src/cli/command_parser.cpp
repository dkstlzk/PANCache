#include "cli/command_parser.hpp"
#include <cctype>
using namespace std;

CommandParser::CommandParser(CacheEngine& engine)
    : engine_(engine) {
    registerCommands();
}

vector<string> CommandParser::tokenize(const string& line) const {
    stringstream ss(line);
    vector<string> tokens;
    string word;

    while (ss >> word)
        tokens.push_back(word);

    return tokens;
}

void CommandParser::registerCommands() {
    commands_ = {
        {"SET",    [this](const auto& a){ return cmdSet(a); }},
        {"GET",    [this](const auto& a){ return cmdGet(a); }},
        {"DEL",    [this](const auto& a){ return cmdDel(a); }},
        {"LINK",   [this](const auto& a){ return cmdLink(a); }},
        {"EXPIRE", [this](const auto& a){ return cmdExpire(a); }},
        {"SIZE",   [this](const auto& a){ return cmdSize(a); }},
        {"SEARCH", [this](const auto& a){ return cmdSearch(a); }},
        {"PREFIX", [this](const auto& a){ return cmdPrefix(a); }},
        {"TOPK",   [this](const auto& a){ return cmdTopK(a); }},
        {"CLEAR",  [this](const auto& a){ return cmdClear(a); }},
        {"HELP",   [this](const auto&){ return helpText(); }},
        {"EXIT",   [](const auto&){ return string("EXIT is only supported in CLI mode"); }}
    };
}

string CommandParser::handleCommand(const string& input) {
    auto tokens = tokenize(input);
    if (tokens.empty()) return "";

    string cmd = tokens[0];
    for (auto& c : cmd) c = toupper(static_cast<unsigned char>(c));
    auto it = commands_.find(cmd);

    if (it != commands_.end())
        return it->second(tokens);

    return "Unknown command: " + cmd;
}

string CommandParser::helpText() const {
        return R"(
PANCache Commands:
    SET <key> <value>          - Store key-value pair
    GET <key>                  - Retrieve value
    DEL <key>                  - Delete key
    LINK <parent> <child>      - Create dependency edge
    EXPIRE <key> <ttl_seconds> - Set time-to-live
    SIZE                       - Display total items
    SEARCH <word>              - Exact search (checks if key exists)
    PREFIX <pre>               - List keys starting with prefix
    TOPK <k>                   - Show top k frequently accessed keys
    CLEAR                      - Remove all keys
    HELP                       - Show this help
    EXIT                       - Quit (CLI only)
)";
}

string CommandParser::cmdSet(const vector<string>& args) {
    if (args.size() < 3)
        return "Usage: SET <key> <value>";

    string value;
    for (size_t i = 2; i < args.size(); ++i) {
        if (i > 2) value += " ";
        value += args[i];
    }

    engine_.set(args[1], value);
    return "OK";
}

string CommandParser::cmdGet(const vector<string>& args) {
    if (args.size() < 2)
        return "Usage: GET <key>";

    auto value = engine_.get(args[1]);
    if (value.has_value())
        return args[1] + " = " + value.value();

    return "Key not found: " + args[1];
}

string CommandParser::cmdDel(const vector<string>& args) {
    if (args.size() < 2)
        return "Usage: DEL <key>";

    engine_.del(args[1]);
    return "OK";
}

string CommandParser::cmdLink(const vector<string>& args) {
    if (args.size() < 3)
        return "Usage: LINK <parent> <child>";

    engine_.link(args[1], args[2]);
    return "OK";
}

string CommandParser::cmdExpire(const vector<string>& args) {
    if (args.size() < 3)
        return "Usage: EXPIRE <key> <ttl_seconds>";

    try {
        int ttl = stoi(args[2]);
        if (ttl <= 0)
            return "TTL must be > 0";
        engine_.expire(args[1], ttl);
    } catch (const std::exception&) {
        return "TTL must be a number";
    }

    return "OK";
}

string CommandParser::cmdSize(const vector<string>&) {
    engine_.cleanupExpired();
    return "Cache size: " + to_string(engine_.size());
}

string CommandParser::cmdSearch(const vector<string>& args) {
    if (args.size() < 2)
        return "Usage: SEARCH <word>";

    engine_.cleanupExpired();
    bool exists = engine_.contains(args[1]);
    return exists ? "Found" : "Not found";
}

string CommandParser::cmdPrefix(const vector<string>& args) {
    if (args.size() < 2)
        return "Usage: PREFIX <prefix>";

    engine_.cleanupExpired();
    engine_.setLastPrefixQuery(args[1]);
    auto words = engine_.prefix(args[1]);
    if (words.empty())
        return "No matches.";

    string out = "Matches: ";
    for (const auto& w : words) {
        out += w + " ";
    }
    return out;
}

string CommandParser::cmdTopK(const vector<string>& args) {
    if (args.size() < 2)
        return "Usage: TOPK <k>";

    int k = 0;
    try {
        k = stoi(args[1]);
    } catch (const std::exception&) {
        return "k must be a number";
    }

    if (k <= 0) return "k must be > 0";

    engine_.setLastTopK(k);
    auto result = engine_.topK(k);

    if (result.empty()) return "(empty)";

    string out;
    for (const auto& p : result)
        out += p.first + " => " + to_string(p.second) + "\n";
    return out;
}

string CommandParser::cmdClear(const vector<string>&) {
    engine_.clear();
    return "OK";
}
