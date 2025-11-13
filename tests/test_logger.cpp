#include "utils/logger.hpp"
#include <iostream>
#include <cassert>

using namespace PANCache::Utils; 

int main() {
    auto& log = Logger::getInstance();

    log.setLogLevel(Logger::Level::DEBUG);
    log.info("Logger initialized successfully");
    log.debug("Debug message for internal state");
    log.warn("Warning: This is a test warning");
    log.error("Error: Simulated failure condition");

    log.enableFileOutput("logger_test_output.log");
    log.info("This should also appear in the log file");
    log.disableFileOutput();

    std::cout << "\nAll Logger tests executed successfully " << std::endl;
    return 0;
}
