#pragma once
#define CPPHTTPLIB_PARSE_URL_QUERY
#include "httplib.hpp"
#include "../data/cache_engine.hpp"

class CommandParser;

class HttpServer {
public:
    HttpServer(CacheEngine& cache, CommandParser& parser);

    void setupRoutes();
    void start(const std::string& host, int port);
    void stop();

private:
    httplib::Server server_;
    CacheEngine& cache_;
    CommandParser& parser_;  
};