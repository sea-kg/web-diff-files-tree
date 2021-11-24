
#include "webdiff_http_server.h"

#include "WebSocketServer.h"
#include "EventLoop.h"
#include "htime.h"
#include "hssl.h"

using namespace hv;


WebdiffHttpServer::WebdiffHttpServer() {
    TAG = "WebdiffHttpServer";
    m_httpService = new HttpService();

    // static files
    m_httpService->document_root = "./www";

    m_httpService->GET("/api/", std::bind(&WebdiffHttpServer::httpApiV1GetPaths, this, std::placeholders::_1, std::placeholders::_2));
    m_httpService->GET("/api/v1/", std::bind(&WebdiffHttpServer::httpApiV1GetPaths, this, std::placeholders::_1, std::placeholders::_2));

    m_httpService->GET("/get", [](HttpRequest* req, HttpResponse* resp) {
        resp->json["origin"] = req->client_addr.ip;
        resp->json["url"] = req->url;
        resp->json["args"] = req->query_params;
        resp->json["headers"] = req->headers;
        return 200;
    });
}

HttpService *WebdiffHttpServer::getService() {
    return m_httpService;
}

int WebdiffHttpServer::httpApiV1GetPaths(HttpRequest* req, HttpResponse* resp) {
    return resp->Json(m_httpService->Paths());
}
