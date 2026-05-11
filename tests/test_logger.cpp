#include "utils/logger.hpp"
#include <cassert>
#include <fstream>
#include <cstdio>

using namespace PANCache::Utils;
using namespace std;

int main() {
    auto& log = Logger::getInstance();
    log.setLogLevel(Logger::Level::DEBUG);

    log.debug("Logger debug test");
    log.info("Logger info test");
    log.warn("Logger warn test");
    log.error("Logger error test");

    const string filename = "logger_test.log";
    bool ok = log.enableFileOutput(filename);
    assert(ok);

    log.info("Logger file output test");
    log.disableFileOutput();

    ifstream file(filename);
    assert(file.good());

    string line;
    getline(file, line);
    assert(!line.empty());
    file.close();

    remove(filename.c_str());

    return 0;
}
