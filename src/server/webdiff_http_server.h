#ifndef VV_HTTP_SERVER_H
#define VV_HTTP_SERVER_H

#include <string>
#include <json.hpp>
#include "HttpService.h"

using namespace hv;

class WebdiffHttpServer {
    public:
        WebdiffHttpServer();
        HttpService *getService();
        int httpApiV1GetPaths(HttpRequest* req, HttpResponse* resp);
        
        // support previous api
        int httpApiAdd(HttpRequest* req, HttpResponse* resp);
        int httpApiCommentAdd(HttpRequest* req, HttpResponse* resp);
        int httpApiCommentHide(HttpRequest* req, HttpResponse* resp);
        int httpApiDiff(HttpRequest* req, HttpResponse* resp);
        int httpApiFiles(HttpRequest* req, HttpResponse* resp);
        int httpApiGroups(HttpRequest* req, HttpResponse* resp);
        int httpApiGroupsAll(HttpRequest* req, HttpResponse* resp);
        int httpApiVersionsAll(HttpRequest* req, HttpResponse* resp);

    private:
        static int response_status(HttpResponse* resp, int code = 200, const char* message = NULL) {
            if (message == NULL) message = http_status_str((enum http_status)code);
            resp->Set("code", code);
            resp->Set("message", message);
            return code;
        }

        std::string TAG;
        HttpService *m_httpService;
};

#endif // VV_HTTP_SERVER_H