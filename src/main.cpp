#include "data/cache_engine.hpp"
#include "cli/command_parser.hpp"
#include "utils/http_server.hpp"
#include "utils/logger.hpp"
#include <csignal>

namespace {
HttpServer* g_server = nullptr;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        auto& log = PANCache::Utils::Logger::getInstance();
        log.info("Received shutdown signal");
        if (g_server) g_server->stop();
    }
}
}

int main() {
    using namespace PANCache::Utils;
    
    auto& log = Logger::getInstance();
    log.setLogLevel(Logger::Level::INFO);
    
    log.info("Initializing PANCache...");
    
    CacheEngine cache(50);
    log.info("Cache engine initialized with capacity: 50");
    
    CommandParser cli(cache);
    log.debug("Command parser initialized");
    
    HttpServer http(cache, cli);
    log.debug("HTTP server initialized");
    g_server = &http;

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    log.debug("Signal handlers registered");

    http.setupRoutes();
    log.info("HTTP routes configured");

    const std::string host = "127.0.0.1";
    const int port = 8080;

    log.info("PANCache server starting on http://" + host + ":" + std::to_string(port));
    log.info("Available endpoints:");
    log.info("  - GET  /health");
    log.info("  - POST /cmd");
    log.info("Press Ctrl+C to stop the server");

    if (!http.start(host, port)) {
        log.error("HTTP server failed to start");
        return 1;
    }

    log.info("PANCache server stopped successfully");
    log.disableFileOutput();
    
    return 0;
}