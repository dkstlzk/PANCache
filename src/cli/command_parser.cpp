#include "cli/command_parser.hpp"
#include <iomanip>
#include <algorithm>

using namespace PANCache::CLI;
using namespace std;

CommandParser::CommandParser(Data::CacheEngine& engine) : engine_(engine) {
    registerCommands();
}

vector<string> CommandParser::tokenize(const string& line) const {
    istringstream iss(line);
    vector<string> tokens;
    string word;
    while (iss >> word)
        tokens.push_back(word);
    return tokens;
}

void CommandParser::registerCommands() {
    commands_ = {
        {"SET",     [this](const auto& a){ cmdSet(a); }},
        {"GET",     [this](const auto& a){ cmdGet(a); }},
        {"DEL",     [this](const auto& a){ cmdDel(a); }},
        {"LINK",    [this](const auto& a){ cmdLink(a); }},
        {"EXPIRE",  [this](const auto& a){ cmdExpire(a); }},
        {"SIZE",    [this](const auto& a){ cmdSize(a); }},
        {"HELP",    [this](const auto& a){ printHelp(); }},
        {"EXIT",    [](const auto&){ cout << "Bye 👋" << endl; exit(0); }}
    };
}

void CommandParser::handleCommand(const string& input) {
    auto tokens = tokenize(input);
    if (tokens.empty()) return;

    string cmd = tokens[0];
    transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    if (commands_.find(cmd) != commands_.end()) {
        commands_[cmd](tokens);
    } else {
        cout << "❌ Unknown command. Type HELP for a list.\n";
    }
}

// ------------------- Command Implementations -------------------

void CommandParser::cmdSet(const vector<string>& args) {
    if (args.size() < 3) {
        cout << "Usage: SET <key> <value> [ttl_seconds]\n";
        return;
    }
    string key = args[1];
    string value = args[2];
    int ttl = (args.size() >= 4) ? stoi(args[3]) : 0;
    engine_.set(key, value, ttl);
    cout << "✅ Key '" << key << "' set.\n";
}

void CommandParser::cmdGet(const vector<string>& args) {
    if (args.size() != 2) {
        cout << "Usage: GET <key>\n";
        return;
    }
    auto val = engine_.get(args[1]);
    if (val.has_value())
        cout << args[1] << " = " << val.value() << "\n";
    else
        cout << "(nil)\n";
}

void CommandParser::cmdDel(const vector<string>& args) {
    if (args.size() != 2) {
        cout << "Usage: DEL <key>\n";
        return;
    }
    bool ok = engine_.del(args[1]);
    cout << (ok ? "🗑️  Key deleted.\n" : "⚠️  Key not found.\n");
}

void CommandParser::cmdLink(const vector<string>& args) {
    if (args.size() != 3) {
        cout << "Usage: LINK <parent> <child>\n";
        return;
    }
    engine_.depend(args[1], args[2]);
    cout << "🔗 Linked " << args[1] << " → " << args[2] << "\n";
}

void CommandParser::cmdExpire(const vector<string>& args) {
    if (args.size() != 3) {
        cout << "Usage: EXPIRE <key> <ttl_seconds>\n";
        return;
    }
    int ttl = stoi(args[2]);
    bool ok = engine_.expire(args[1], ttl);
    cout << (ok ? "⏳ TTL updated.\n" : "⚠️  Key not found.\n");
}

void CommandParser::cmdSize(const vector<string>&) {
    cout << "📦 Cache size: " << engine_.size() << " entries.\n";
}

void CommandParser::printHelp() const {
    cout << "\n🧠 PANCache CLI Commands:\n"
         << "  SET <key> <value> [ttl]   - Store a key with optional TTL\n"
         << "  GET <key>                 - Retrieve value\n"
         << "  DEL <key>                 - Delete key\n"
         << "  LINK <parent> <child>     - Create dependency link\n"
         << "  EXPIRE <key> <seconds>    - Reset TTL of key\n"
         << "  SIZE                      - Show total cached keys\n"
         << "  HELP                      - Show this help menu\n"
         << "  EXIT                      - Exit CLI\n\n";
}
