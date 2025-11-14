#include "data/cache_engine.hpp"
#include "cli/command_parser.hpp"
#include "utils/http_server.hpp"
#include "utils/logger.hpp"
#include <csignal>

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        auto& log = PANCache::Utils::Logger::getInstance();
        log.info("Received shutdown signal");
    }
}

int main() {
    using namespace PANCache::Utils;
    
    auto& log = Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);
    
    printf("Initializing PANCache...");
    
    CacheEngine cache(50);
    log.info("Cache engine initialized with capacity: 50");
    
    CommandParser cli(cache);
    log.debug("Command parser initialized");
    
    HttpServer http(cache, cli);
    log.debug("HTTP server initialized");

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    log.debug("Signal handlers registered");

    http.setupRoutes();
    log.info("HTTP routes configured");

    log.info("PANCache server starting on http://0.0.0.0:8080");
    log.info("Available endpoints:");
    log.info("  - GET  /health");
    log.info("  - POST /cmd");
    log.info("Press Ctrl+C to stop the server");

    http.start("0.0.0.0", 8080);

    log.info("PANCache server stopped successfully");
    log.disableFileOutput();
    
    return 0;
}