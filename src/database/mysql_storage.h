#ifndef MYSQL_STORAGE_H
#define MYSQL_STORAGE_H

#include <map>
#include <mysql/mysql.h>
#include <mutex>
#include <vector>
#include "model_version.h"

class MySqlStorage;

class MySqlStorageConnection {
    public:
        MySqlStorageConnection(MYSQL *pConn, MySqlStorage *pStorage);
        ~MySqlStorageConnection();
        bool executeQuery(const std::string &sQuery);
        std::string lastDatabaseVersion();
        std::vector<std::string> getInstalledVersions();
        bool insertUpdateInfo(const std::string &sVersion, const std::string &sDescription);
        long getConnectionDurationInSeconds();
        std::vector<ModelVersion> getApiVersionsAll();
    private:
        std::string TAG;
        MYSQL *m_pConnection;
        MySqlStorage *m_pStorage;
        std::mutex m_mtxConn;
        long m_nCreated;
};

class MySqlStorage {
    public:
        MySqlStorage();
        MySqlStorageConnection *connect();
        std::string prepareStringValue(const std::string &sValue);
        MySqlStorageConnection * getConnection();
        bool loadCache();

    private:
        std::string TAG;
        std::string m_sDatabaseHost;
        std::string m_sDatabaseName;
        std::string m_sDatabaseUser;
        std::string m_sDatabasePass;
        int m_nDatabasePort;
        int m_nConnectionOutdatedAfterSeconds;

        std::map<std::string, MySqlStorageConnection*> m_mapStorageConnections;
        std::vector<MySqlStorageConnection*> m_vDoRemoveStorageConnections;
        std::mutex m_mtxStorageConnections;
};



#endif // MYSQL_STORAGE_H
