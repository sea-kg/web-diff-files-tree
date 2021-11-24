#ifndef VV_HTTP_SERVER_H
#define VV_HTTP_SERVER_H

#include <string>
#include <json.hpp>
#include "HttpService.h"

class WebdiffHttpServer {
    public:
        WebdiffHttpServer();
        HttpService *getService();
        int httpApiV1GetPaths(HttpRequest* req, HttpResponse* resp);

    private:
        std::string TAG;
        HttpService *m_httpService;
};

#endif // VV_HTTP_SERVER_H