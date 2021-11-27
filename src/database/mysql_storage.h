#ifndef MYSQL_STORAGE_H
#define MYSQL_STORAGE_H

#include <map>
#include <mysql/mysql.h>
#include <mutex>
#include <vector>
#include "model_version.h"

class MySqlStorage;

class MySqlStorageConnection {
    friend MySqlStorage;
    public:
        MySqlStorageConnection(MYSQL *pConn);
        ~MySqlStorageConnection();
        bool executeQuery(const std::string &sQuery);
        std::string lastDatabaseVersion();
        std::vector<std::string> getInstalledVersions();
        bool insertUpdateInfo(const std::string &sVersion, const std::string &sDescription);
        long getConnectionDurationInSeconds();
        std::string prepareStringValue(const std::string &sValue);

    protected:
        std::vector<ModelVersion> getApiVersionsAll();

    private:
        std::string TAG;
        MYSQL *m_pConnection;
        std::mutex m_mtxConn;
        long m_nCreated;
};

class MySqlStorage {
    public:
        MySqlStorage();
        MySqlStorageConnection * getConnection();
        bool loadCache();
        const std::vector<ModelVersion> &getApiVersionsAll();

    private:
        MySqlStorageConnection *connect();

        std::string TAG;
        std::string m_sDatabaseHost;
        std::string m_sDatabaseName;
        std::string m_sDatabaseUser;
        std::string m_sDatabasePass;
        int m_nDatabasePort;

        // connections
        int m_nConnectionOutdatedAfterSeconds;
        std::map<std::string, MySqlStorageConnection*> m_mapStorageConnections;
        std::vector<MySqlStorageConnection*> m_vDoRemoveStorageConnections;
        std::mutex m_mtxStorageConnections;

        // caches
        std::vector<ModelVersion> m_vVersions;
};



#endif // MYSQL_STORAGE_H
