#include "data/cache_engine.hpp"
#include "cli/command_parser.hpp"
#include "utils/http_server.hpp"
#include "utils/logger.hpp"
#include <csignal>
#include <thread>
#include <cstdlib>
#include <cctype>
#include <algorithm>

namespace {
HttpServer* g_server = nullptr;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        auto& log = PANCache::Utils::Logger::getInstance();
        log.info("Received shutdown signal");
        if (g_server) g_server->stop();
    }
}

bool shouldAutoOpenBrowser() {
    const char* raw = std::getenv("PANCACHE_OPEN_BROWSER");
    if (!raw) return true;
    std::string v(raw);
    std::transform(v.begin(), v.end(), v.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return !(v == "0" || v == "false" || v == "off" || v == "no");
}

int launchBrowser(const std::string& url) {
#if defined(_WIN32)
    std::string cmd = "cmd /c start \"\" \"" + url + "\"";
#elif defined(__APPLE__)
    std::string cmd = "open \"" + url + "\"";
#else
    std::string cmd = "xdg-open \"" + url + "\"";
#endif
    return std::system(cmd.c_str());
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

    const std::string url = "http://" + host + ":" + std::to_string(port);
    log.info("PANCache server starting on " + url);
    log.info("Available endpoints:");
    log.info("  - GET  /health");
    log.info("  - POST /cmd");
    log.info("PANCache UI available at: " + url);
    log.info("Press Ctrl+C to stop the server");

    if (shouldAutoOpenBrowser()) {
        log.info("Auto-open browser enabled");
        std::thread([&http, url]() {
            http.waitUntilReady();
            if (!http.isRunning()) return;
            auto& log = Logger::getInstance();
            log.info("Opening browser: " + url);
            if (launchBrowser(url) != 0) {
                log.warn("Browser launch command failed");
            }
        }).detach();
    } else {
        log.info("Auto-open browser disabled via PANCACHE_OPEN_BROWSER");
    }

    if (!http.start(host, port)) {
        log.error("HTTP server failed to start");
        return 1;
    }

    log.info("PANCache server stopped successfully");
    log.disableFileOutput();
    
    return 0;
}