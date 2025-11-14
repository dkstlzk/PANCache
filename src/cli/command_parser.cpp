#include "cli/command_parser.hpp"
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
        {"SET",    [this](const auto& a){ cmdSet(a); }},
        {"GET",    [this](const auto& a){ cmdGet(a); }},
        {"DEL",    [this](const auto& a){ cmdDel(a); }},
        {"LINK",   [this](const auto& a){ cmdLink(a); }},
        {"EXPIRE", [this](const auto& a){ cmdExpire(a); }},
        {"SIZE",   [this](const auto& a){ cmdSize(a); }},
        {"SEARCH", [this](const auto& a){ cmdSearch(a); }},
        {"PREFIX", [this](const auto& a){ cmdPrefix(a); }},
        {"TOPK",   [this](const auto& a){ cmdTopK(a); }},
        {"HELP",   [this](const auto&){ printHelp(); }},
        {"EXIT",   [](const auto&){ cout << "Exiting PANCache...\n"; exit(0); }}
    };
}

void CommandParser::handleCommand(const string& input) {
    auto tokens = tokenize(input);
    if (tokens.empty()) return;

    string cmd = tokens[0];
    auto it = commands_.find(cmd);

    if (it != commands_.end())
        it->second(tokens);
    else
        cout << "Unknown command: " << cmd << "\n";
}

void CommandParser::printHelp() const {
    cout << R"(
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
  HELP                       - Show this help
  EXIT                       - Quit
)";
}

void CommandParser::cmdSet(const vector<string>& args) {
    if (args.size() < 3) {
        cout << "Usage: SET <key> <value>\n";
        return;
    }
    engine_.set(args[1], args[2]);
}

void CommandParser::cmdGet(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "Usage: GET <key>\n";
        return;
    }

    auto value = engine_.get(args[1]);
    if (value.has_value())
        cout << args[1] << " = " << value.value() << "\n";
    else
        cout << "Key not found: " << args[1] << "\n";
}

void CommandParser::cmdDel(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "Usage: DEL <key>\n";
        return;
    }
    engine_.del(args[1]);
}

void CommandParser::cmdLink(const vector<string>& args) {
    if (args.size() < 3) {
        cout << "Usage: LINK <parent> <child>\n";
        return;
    }
    engine_.link(args[1], args[2]);
}

void CommandParser::cmdExpire(const vector<string>& args) {
    if (args.size() < 3) {
        cout << "Usage: EXPIRE <key> <ttl_seconds>\n";
        return;
    }

    int ttl= stoi(args[2]);
    engine_.expire(args[1], ttl);
}

void CommandParser::cmdSize(const vector<string>&) {
    cout << "Cache size: " << engine_.size() << "\n";
}

void CommandParser::cmdSearch(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "Usage: SEARCH <word>\n";
        return;
    }

    bool exists= engine_.get(args[1]).has_value();  // exact key check
    cout << (exists? "Found\n" : "Not found\n");
}

void CommandParser::cmdPrefix(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "Usage: PREFIX <prefix>\n";
        return;
    }

    auto words= engine_.prefix(args[1]);  // trie prefix query
    if (words.empty()) {
        cout << "No matches.\n";
        return;
    }

    cout << "Matches: ";
    for (auto& w: words) cout << w << " ";
    cout << "\n";
}

void CommandParser::cmdTopK(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "Usage: TOPK <k>\n";
        return;
    }

    int k= stoi(args[1]);
    auto result= engine_.topK(k);

    for (auto& p : result)
        cout << p.first << " => " << p.second << "\n";
}
