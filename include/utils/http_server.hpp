#include <nlohmann/json.hpp>

#include "data/cache_engine_state.hpp"

#pragma once
#define CPPHTTPLIB_PARSE_URL_QUERY
#include "httplib.hpp"
#include "../data/cache_engine.hpp"

std::string to_json(const CacheEngineState&);

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