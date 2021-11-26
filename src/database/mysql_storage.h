#ifndef MYSQL_STORAGE_H
#define MYSQL_STORAGE_H

#include <map>
#include <mysql/mysql.h>
#include <mutex>
#include <vector>

class MySqlStorage;

class MySqlStorageConnection {
    public:
        MySqlStorageConnection(MYSQL *pConn, MySqlStorage *pStorage);
        virtual ~MySqlStorageConnection();
        virtual bool executeQuery(const std::string &sQuery);
        virtual std::string lastDatabaseVersion();
        virtual std::vector<std::string> getInstalledVersions();
        virtual bool insertUpdateInfo(const std::string &sVersion, const std::string &sDescription);
        virtual std::vector<std::string> getApiVersionsAll();
    private:
        std::string TAG;
        MYSQL *m_pConnection;
        MySqlStorage *m_pStorage;
        std::mutex m_mtxConn;
};


class MySqlStorage {
    public:
        MySqlStorage();
        MySqlStorageConnection *connect();
        std::string prepareStringValue(const std::string &sValue);
        
    private:
        std::string TAG;
        std::string m_sDatabaseHost;
        std::string m_sDatabaseName;
        std::string m_sDatabaseUser;
        std::string m_sDatabasePass;
        int m_nDatabasePort;
};



#endif // MYSQL_STORAGE_H
