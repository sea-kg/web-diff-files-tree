#ifndef MYSQL_STORAGE_H
#define MYSQL_STORAGE_H

#include <map>
#include <mysql/mysql.h>
#include <mutex>
#include <vector>
#include "model_version.h"
#include "model_group.h"

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
        

    protected:
        long getConnectionDurationInSeconds();
        std::vector<ModelVersion> getApiVersionsAll();
        std::vector<ModelGroup> getGroupsAll();

    private:
        std::string prepareStringValue(const std::string &sValue);

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
        const std::vector<ModelVersion> &getVersionsAll();
        const std::vector<ModelGroup> &getGroupsAll();

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
        std::vector<ModelGroup> m_vGroups;
};



#endif // MYSQL_STORAGE_H
