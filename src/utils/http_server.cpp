#include "utils/http_server.hpp"
#include "cli/command_parser.hpp"
#include "data/cache_engine_state.hpp"
#include <iostream>
#include <sstream>

HttpServer::HttpServer(CacheEngine& cache, CommandParser& parser)
    : cache_(cache), parser_(parser) {}

void HttpServer::setupRoutes() {

    // ---------- Logging middleware ----------
    server_.set_logger([](const httplib::Request& req, const httplib::Response& res) {
        std::cout << "[HTTP] " << req.method << " " << req.path
                  << " => " << res.status << "\n";
    });

    // ---------- HEALTH CHECK ----------
    server_.Get("/health", [&](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
        res.set_content("OK", "text/plain");
    });

    // ---------- HANDLE CACHE COMMAND ----------
    server_.Post("/cmd", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.body.empty()) {
            res.status = 400;
            res.set_content("Missing command body", "text/plain");
            return;
        }

        std::string cmd = req.body;

        // Capture CLI output safely
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        parser_.handleCommand(cmd);

        std::cout.rdbuf(old);

        res.set_content(buffer.str(), "text/plain");
        res.status = 200;
    });

    // ---------- EXPORT FULL CACHE STATE ----------
    server_.Get("/state", [&](const httplib::Request&, httplib::Response& res) {

        CacheEngineState st = cache_.exportState();  // real function
        std::string jsonStr = to_json(st);          // serialize to JSON

        res.set_content(jsonStr, "application/json");
        res.status = 200;
    });
}

void HttpServer::start(const std::string& host, int port) {
    std::cout << "[HTTP] Listening on " << host << ":" << port << "\n";
    server_.listen(host.c_str(), port);
}

void HttpServer::stop() {
    server_.stop();
}
