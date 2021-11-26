
#include "webdiff_http_server.h"

#include "WebSocketServer.h"
#include "EventLoop.h"
#include "htime.h"
#include "hssl.h"

#include "hbase.h"
#include "htime.h"
#include "hfile.h"
#include "hstring.h"
#include "EventLoop.h" // import setTimeout, setInterval

using namespace hv;

WebdiffHttpServer::WebdiffHttpServer(MySqlStorage *pStorage) {
    TAG = "WebdiffHttpServer";
    m_httpService = new HttpService();
    m_pStorage = pStorage;

    // static files
    m_httpService->document_root = "./html";

    m_httpService->GET("/api/", std::bind(&WebdiffHttpServer::httpApiV1GetPaths, this, std::placeholders::_1, std::placeholders::_2));
    m_httpService->GET("/api/v1/", std::bind(&WebdiffHttpServer::httpApiV1GetPaths, this, std::placeholders::_1, std::placeholders::_2));
    
    // previous api
    m_httpService->POST("/api/add/", std::bind(&WebdiffHttpServer::httpApiAdd, this, std::placeholders::_1, std::placeholders::_2));
    m_httpService->POST("/api/comment-add/", std::bind(&WebdiffHttpServer::httpApiCommentAdd, this, std::placeholders::_1, std::placeholders::_2));
    m_httpService->POST("/api/comment-hide/", std::bind(&WebdiffHttpServer::httpApiCommentHide, this, std::placeholders::_1, std::placeholders::_2));
    m_httpService->POST("/api/diff/", std::bind(&WebdiffHttpServer::httpApiDiff, this, std::placeholders::_1, std::placeholders::_2));
    m_httpService->POST("/api/files/", std::bind(&WebdiffHttpServer::httpApiFiles, this, std::placeholders::_1, std::placeholders::_2));
    m_httpService->POST("/api/groups/", std::bind(&WebdiffHttpServer::httpApiGroups, this, std::placeholders::_1, std::placeholders::_2));
    m_httpService->POST("/api/groups-all/", std::bind(&WebdiffHttpServer::httpApiGroupsAll, this, std::placeholders::_1, std::placeholders::_2));
    m_httpService->POST("/api/versions-all/", std::bind(&WebdiffHttpServer::httpApiVersionsAll, this, std::placeholders::_1, std::placeholders::_2));

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

int WebdiffHttpServer::httpApiAdd(HttpRequest* req, HttpResponse* resp) {
    if (req->content_type != APPLICATION_JSON) {
        return response_status(resp, HTTP_STATUS_BAD_REQUEST);
    }
    resp->content_type = APPLICATION_JSON;
    req->ParseBody();

    // resp->json = req->json;
    resp->json["req_body"] = req->body;
    resp->json["req_json"] = req->json;
    resp->json["int"] = 123;
    resp->json["float"] = 3.14;
    resp->json["string"] = "hello";
    resp->json["url"] = req->url;
    return 200;
}

int WebdiffHttpServer::httpApiCommentAdd(HttpRequest* req, HttpResponse* resp) {
    if (req->content_type != APPLICATION_JSON) {
        return response_status(resp, HTTP_STATUS_BAD_REQUEST);
    }
    resp->content_type = APPLICATION_JSON;
    req->ParseBody();

    // resp->json = req->json;
    resp->json["req_body"] = req->body;
    resp->json["req_json"] = req->json;
    resp->json["int"] = 123;
    resp->json["float"] = 3.14;
    resp->json["string"] = "hello";
    resp->json["url"] = req->url;
    return 200;
}

int WebdiffHttpServer::httpApiCommentHide(HttpRequest* req, HttpResponse* resp) {
    if (req->content_type != APPLICATION_JSON) {
        return response_status(resp, HTTP_STATUS_BAD_REQUEST);
    }
    resp->content_type = APPLICATION_JSON;
    req->ParseBody();

    // resp->json = req->json;
    resp->json["req_body"] = req->body;
    resp->json["req_json"] = req->json;
    resp->json["int"] = 123;
    resp->json["float"] = 3.14;
    resp->json["string"] = "hello";
    resp->json["url"] = req->url;
    return 200;
}

int WebdiffHttpServer::httpApiDiff(HttpRequest* req, HttpResponse* resp) {
    if (req->content_type != APPLICATION_JSON) {
        return response_status(resp, HTTP_STATUS_BAD_REQUEST);
    }
    resp->content_type = APPLICATION_JSON;
    req->ParseBody();

    // resp->json = req->json;
    resp->json["req_body"] = req->body;
    resp->json["req_json"] = req->json;
    resp->json["int"] = 123;
    resp->json["float"] = 3.14;
    resp->json["string"] = "hello";
    resp->json["url"] = req->url;
    return 200;
}
        
int WebdiffHttpServer::httpApiFiles(HttpRequest* req, HttpResponse* resp) {
    if (req->content_type != APPLICATION_JSON) {
        return response_status(resp, HTTP_STATUS_BAD_REQUEST);
    }
    resp->content_type = APPLICATION_JSON;
    req->ParseBody();

    // resp->json = req->json;
    resp->json["req_body"] = req->body;
    resp->json["req_json"] = req->json;
    resp->json["int"] = 123;
    resp->json["float"] = 3.14;
    resp->json["string"] = "hello";
    resp->json["url"] = req->url;
    return 200;
}

int WebdiffHttpServer::httpApiGroups(HttpRequest* req, HttpResponse* resp) {
    if (req->content_type != APPLICATION_JSON) {
        return response_status(resp, HTTP_STATUS_BAD_REQUEST);
    }
    resp->content_type = APPLICATION_JSON;
    req->ParseBody();

    // resp->json = req->json;
    resp->json["req_body"] = req->body;
    resp->json["req_json"] = req->json;
    resp->json["int"] = 123;
    resp->json["float"] = 3.14;
    resp->json["string"] = "hello";
    resp->json["url"] = req->url;
    return 200;
}

int WebdiffHttpServer::httpApiGroupsAll(HttpRequest* req, HttpResponse* resp) {
    if (req->content_type != APPLICATION_JSON) {
        return response_status(resp, HTTP_STATUS_BAD_REQUEST);
    }
    resp->content_type = APPLICATION_JSON;
    req->ParseBody();

    // resp->json = req->json;
    resp->json["req_body"] = req->body;
    resp->json["req_json"] = req->json;
    resp->json["int"] = 123;
    resp->json["float"] = 3.14;
    resp->json["string"] = "hello";
    resp->json["url"] = req->url;
    return 200;
}

int WebdiffHttpServer::httpApiVersionsAll(HttpRequest* req, HttpResponse* resp) {
    if (req->content_type != APPLICATION_JSON) {
        return response_status(resp, HTTP_STATUS_BAD_REQUEST);
    }
    resp->content_type = APPLICATION_JSON;
    req->ParseBody();

    MySqlStorageConnection *pConn = m_pStorage->connect();

    std::vector<ModelVersion> vVersions = pConn->getApiVersionsAll();
    nlohmann::json jsonVersions = nlohmann::json::array();
    for (int i = 0; i < vVersions.size(); i++) {
        jsonVersions.push_back(vVersions[i].toJson());
    }

    // resp->json = req->json;
    resp->json["req_body"] = req->body;
    resp->json["req_json"] = req->json;
    resp->json["list"] = jsonVersions;
    
    resp->json["int"] = 123;
    resp->json["float"] = 3.14;
    resp->json["string"] = "hello";
    resp->json["url"] = req->url;
    return 200;
}
