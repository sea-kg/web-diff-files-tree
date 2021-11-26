
#include "webdiff_server.h"
#include "webdiff_ws_server.h"
#include "webdiff_http_server.h"

#include "WebSocketServer.h"
#include "EventLoop.h"
#include "htime.h"
#include "hssl.h"

using namespace hv;


WebdiffServer::WebdiffServer(MySqlStorage *pStorage) {
    TAG = "WebdiffServer";
    m_pStorage = pStorage;
}

void WebdiffServer::startSync(int nPort) {
    WebdiffWsServer wsServer(m_pStorage);
    WebSocketService *pWs = wsServer.getService();
    WebdiffHttpServer httpServer(m_pStorage);
    HttpService *pRouter = httpServer.getService();

    websocket_server_t server;
    server.service = pRouter;
    server.port = nPort;
    server.ws = pWs;
    websocket_server_run(&server);
}