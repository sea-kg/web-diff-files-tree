#ifndef VV_SERVER_H
#define VV_SERVER_H

#include <string>
#include <json.hpp>
#include "mysql_storage.h"

class WebdiffServer {
    public:
        WebdiffServer(MySqlStorage *pStorage);
        void startSync(int nPort);

    private:
        std::string TAG;
        MySqlStorage *m_pStorage;
};

#endif // VV_SERVER_H