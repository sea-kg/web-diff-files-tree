#ifndef VV_SERVER_H
#define VV_SERVER_H

#include <string>
#include <json.hpp>

class WebdiffServer {
    public:
        WebdiffServer();
        void startSync(int nPort);

    private:
        std::string TAG;
};

#endif // VV_SERVER_H