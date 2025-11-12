#include "utils/logger.hpp"

namespace PANCache {
namespace Utils {

static const char* RESET  = "\033[0m";
static const char* RED    = "\033[31m";
static const char* YELLOW = "\033[33m";
static const char* GREEN  = "\033[32m";
static const char* MAGENTA= "\033[35m";

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : currentLevel(Level::INFO), fileOutputEnabled(false) {}
Logger::~Logger() {
    if (logFile.is_open()) logFile.close();
}

void Logger::setLogLevel(Level level) {
    std::lock_guard<std::mutex> lk(mtx);
    currentLevel = level;
}

bool Logger::enableFileOutput(const std::string& filename) {
    std::lock_guard<std::mutex> lk(mtx);
    if (logFile.is_open()) logFile.close();
    logFile.open(filename, std::ios::app);
    fileOutputEnabled = logFile.is_open();
    return fileOutputEnabled;
}

void Logger::disableFileOutput() {
    std::lock_guard<std::mutex> lk(mtx);
    if (logFile.is_open()) logFile.close();
    fileOutputEnabled = false;
}

std::string Logger::timestamp() const {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm_snapshot;
#if defined(_WIN32)
    localtime_s(&tm_snapshot, &t);
#else
    localtime_r(&t, &tm_snapshot);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_snapshot, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

std::string Logger::levelToString(Level level) const {
    switch (level) {
        case Level::DEBUG: return "DEBUG";
        case Level::INFO:  return "INFO";
        case Level::WARN:  return "WARN";
        case Level::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

std::string Logger::colorize(Level level, const std::string& text) const {
    switch (level) {
        case Level::DEBUG: return std::string(MAGENTA) + text + RESET;
        case Level::INFO:  return std::string(GREEN)   + text + RESET;
        case Level::WARN:  return std::string(YELLOW)  + text + RESET;
        case Level::ERROR: return std::string(RED)     + text + RESET;
        default:           return text;
    }
}

void Logger::log(Level level, const std::string& msg) {
    std::lock_guard<std::mutex> lk(mtx);
    if (level < currentLevel) return;

    std::string line = timestamp() + " [" + levelToString(level) + "] " + msg;
    std::cout << colorize(level, line) << std::endl;

    if (fileOutputEnabled && logFile.is_open()) {
        logFile << line << std::endl;
        logFile.flush();
    }
}

void Logger::debug(const std::string& msg) { log(Level::DEBUG, msg); }
void Logger::info (const std::string& msg) { log(Level::INFO,  msg); }
void Logger::warn (const std::string& msg) { log(Level::WARN,  msg); }
void Logger::error(const std::string& msg) { log(Level::ERROR, msg); }

} // namespace Utils
} // namespace PANCache
