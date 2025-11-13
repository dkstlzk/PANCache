#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace PANCache {
namespace Utils {

class Logger {
public:
    enum class Level { DEBUG = 0, INFO, WARN, ERROR };

    static Logger& getInstance();

    void log(Level level, const std::string& msg);
    void setLogLevel(Level level);

    bool enableFileOutput(const std::string& filename);
    void disableFileOutput();

    void debug(const std::string& msg);
    void info (const std::string& msg);
    void warn (const std::string& msg);
    void error(const std::string& msg);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string timestamp() const;
    std::string levelToString(Level level) const;
    std::string colorize(Level level, const std::string& text) const;

    Level currentLevel;
    bool fileOutputEnabled;
    std::ofstream logFile;
    mutable std::mutex mtx;
};

} 
} 
