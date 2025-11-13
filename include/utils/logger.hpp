#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
#include <sstream>
#include <iomanip>
using namespace std;

namespace PANCache {
namespace Utils {

class Logger {
public:
    enum class Level { DEBUG = 0, INFO, WARN, ERROR };

    static Logger& getInstance();

    void log(Level level, const string& msg);
    void setLogLevel(Level level);

    bool enableFileOutput(const string& filename);
    void disableFileOutput();

    void debug(const string& msg);
    void info (const string& msg);
    void warn (const string& msg);
    void error(const string& msg);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) =delete;
    Logger& operator=(const Logger&) = delete;

    string timestamp() const;
    string levelToString(Level level) const;
    string colorize(Level level, const string& text) const;

    Level currentLevel;
    bool fileOutputEnabled;
    ofstream logFile;
    mutable mutex mtx;
};

} 
} 
