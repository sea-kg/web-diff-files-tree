#include <string.h>
#include <iostream>
#include <algorithm>
#include <wsjcpp_core.h>
#include "webdiff_server.h"
#include "mysql_storage.h"

int main(int argc, const char* argv[]) {
    std::string TAG = "MAIN";
    std::string appName = std::string(WSJCPP_APP_NAME);
    std::string appVersion = std::string(WSJCPP_APP_VERSION);
    if (!WsjcppCore::dirExists(".logs")) {
        WsjcppCore::makeDir(".logs");
    }
    WsjcppLog::setPrefixLogFile("web-diff-files-tree");
    WsjcppLog::setLogDirectory(".logs");
    WsjcppCore::initRandom();

    auto *pStorage = new MySqlStorage();
    // auto *pConnection = pStorage->connect();

    int nPort = 1234;
    std::cout << "Starting on port: http://localhost:" << nPort << "/" << std::endl;

    WebdiffServer server(pStorage);
    server.startSync(nPort);

    return 0;
}

