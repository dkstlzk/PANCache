#include "utils/http_server.hpp"
#include "cli/command_parser.hpp"
#include "data/cache_engine_state.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <filesystem>

namespace {
void applyCors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}
}

HttpServer::HttpServer(CacheEngine& cache, CommandParser& parser)
    : cache_(cache), parser_(parser) {}

void HttpServer::setupRoutes() {

    // ---------- Logging middleware ----------
    server_.set_logger([](const httplib::Request& req, const httplib::Response& res) {
        if (req.path == "/favicon.ico") return;
        std::cout << "[HTTP] " << req.method << " " << req.path
                  << " => " << res.status << "\n";
    });

    server_.set_file_extension_and_mimetype_mapping("html", "text/html");
    server_.set_file_extension_and_mimetype_mapping("css", "text/css");
    server_.set_file_extension_and_mimetype_mapping("js", "application/javascript");

    const std::filesystem::path frontend_dir = std::filesystem::current_path() / "frontend";
    if (!server_.set_mount_point("/", frontend_dir.string())) {
        std::cerr << "[HTTP] Failed to mount frontend directory: "
                  << frontend_dir.string() << "\n";
    }

    server_.Options("/cmd", [&](const httplib::Request&, httplib::Response& res) {
        applyCors(res);
        res.status = 200;
    });
    server_.Options("/state", [&](const httplib::Request&, httplib::Response& res) {
        applyCors(res);
        res.status = 200;
    });
    server_.Options("/health", [&](const httplib::Request&, httplib::Response& res) {
        applyCors(res);
        res.status = 200;
    });

    server_.Get("/favicon.ico", [&](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
        res.set_header("Cache-Control", "public, max-age=86400");
    });

    // ---------- HEALTH CHECK ----------
    server_.Get("/health", [&](const httplib::Request&, httplib::Response& res) {
        applyCors(res);
        res.status = 200;
        res.set_content("OK", "text/plain");
    });

    // ---------- HANDLE CACHE COMMAND ----------
    server_.Post("/cmd", [&](const httplib::Request& req, httplib::Response& res) {
        applyCors(res);
        if (req.body.empty()) {
            res.status = 400;
            res.set_content("Missing command body", "text/plain");
            return;
        }

        std::string cmd = req.body;
        std::string output = parser_.handleCommand(cmd);
        if (output.empty()) output = "OK";

        res.set_content(output, "text/plain");
        res.status = 200;
    });

    // ---------- EXPORT FULL CACHE STATE ----------
    server_.Get("/state", [&](const httplib::Request&, httplib::Response& res) {
        applyCors(res);

        CacheEngineState st = cache_.exportState();
        std::string jsonStr = to_json(st);

        res.set_content(jsonStr, "application/json");
        res.set_header("Cache-Control", "no-store");
        res.status = 200;
    });
}

bool HttpServer::start(const std::string& host, int port) {
    std::cout << "[HTTP] Binding to " << host << ":" << port << "\n";
    if (!server_.bind_to_port(host, port)) {
        std::cerr << "[HTTP] Failed to bind to " << host << ":" << port;
#if defined(_WIN32)
        std::cerr << " (WSA error: " << WSAGetLastError() << ")";
#else
        std::cerr << " (errno: " << errno << " - " << std::strerror(errno) << ")";
#endif
        std::cerr << "\n";
        return false;
    }

    std::cout << "[HTTP] Listening on " << host << ":" << port << "\n";
    return server_.listen_after_bind();
}

void HttpServer::stop() {
    server_.stop();
}

bool HttpServer::isRunning() const {
    return server_.is_running();
}

void HttpServer::waitUntilReady() const {
    server_.wait_until_ready();
}
