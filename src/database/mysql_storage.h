#ifndef MYSQL_STORAGE_H
#define MYSQL_STORAGE_H

#include <map>
#include <mysql/mysql.h>
#include <mutex>
#include <vector>

class MySqlStorageConnection {
    public:
        MySqlStorageConnection(MYSQL *pConn);
        virtual ~MySqlStorageConnection();
        virtual bool executeQuery(const std::string &sQuery);
        virtual std::string lastDatabaseVersion();
        virtual std::vector<std::string> getInstalledVersions();
        virtual bool insertUpdateInfo(const std::string &sVersion, const std::string &sDescription);
    private:
        std::string TAG;
        MYSQL *m_pConnection;
        std::mutex m_mtxConn;
};

#endif // MYSQL_STORAGE_H
