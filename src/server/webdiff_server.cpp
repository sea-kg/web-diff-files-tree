
#include "webdiff_server.h"
#include "webdiff_ws_server.h"
#include "webdiff_http_server.h"

#include "WebSocketServer.h"
#include "EventLoop.h"
#include "htime.h"
#include "hssl.h"

using namespace hv;


WebdiffServer::WebdiffServer() {
    TAG = "WebdiffServer";
}

void WebdiffServer::startSync(int nPort) {
    WebdiffWsServer wsServer;
    WebSocketService *pWs = wsServer.getService();
    WebdiffHttpServer httpServer;
    HttpService *pRouter = httpServer.getService();

    websocket_server_t server;
    server.service = pRouter;
    server.port = nPort;
    server.ws = pWs;
    websocket_server_run(&server);
}