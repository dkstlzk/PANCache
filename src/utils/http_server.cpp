#include "utils/http_server.hpp"
#include "cli/command_parser.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

HttpServer::HttpServer(CacheEngine& cache, CommandParser& parser)
    : cache_(cache), parser_(parser) {}


void HttpServer::setupRoutes() {

    server_.set_logger([](const httplib::Request& req, const httplib::Response& res) {
        std::cout << "[HTTP] " << req.method << " " << req.path << " from " << req.remote_addr;

        if (!req.params.empty()) {
            std::cout << " params:";
            for (const auto &p : req.params)
                std::cout << " " << p.first << "=" << p.second;
        }

        std::cout << " => " << res.status << "\n";
    });


    server_.Get("/health", [&](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
        res.set_content("OK", "text/plain");
    });


    server_.Post("/cmd", [&](const httplib::Request& req, httplib::Response& res) {

        if (req.body.empty()) {
            res.status = 400;
            res.set_content("Missing command body", "text/plain");
            return;
        }

        std::string cmd = req.body;

        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        parser_.handleCommand(cmd);

        std::cout.rdbuf(old);

        res.set_content(buffer.str(), "text/plain");
    });
}

void HttpServer::start(const std::string& host, int port) {
    std::cout << "[HTTP] Listening on " << host << ":" << port << "\n";
    server_.listen(host.c_str(), port);
}

void HttpServer::stop() {
    server_.stop();
}
