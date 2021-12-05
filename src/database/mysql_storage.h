#ifndef MYSQL_STORAGE_H
#define MYSQL_STORAGE_H

#include <map>
#include <mysql/mysql.h>
#include <mutex>
#include <vector>
#include "models_basic.h"
#include "models_diff.h"

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
        std::vector<ModelGroupForVersion> getGroups(int nVersionId);
        std::vector<ModelComment> getComments(int nDefineFileId);
        std::vector<ModelFile> getFiles(int nVersionId, int nGroupId, int nParentId);
        void findAndAddFile(const ModelGroup &group, int nFileId, ModelDiffGroups &diffGroups, std::vector<int> &vParentFileIds);
        void getDiffFiles(int nLeftVersionId, int nRightVersionId, const std::string &sState, ModelDiffGroups &diffGroups);
        ModelComment addComment(int nDefineFileId, const std::string &sComment);
        void hideComment(int nCommentId);

    private:
        std::string prepareStringValue(const std::string &sValue);
        int paramtoInt(const char *);
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
        ModelVersion getVersionInfo(int nVersionId);
        const std::vector<ModelGroup> &getGroupsAll();
        std::vector<ModelGroupForVersion> getGroups(int nVersionId);
        std::vector<ModelFile> getFiles(int nVersionId, int nGroupId, int nParentId);
        void getDiff(int nLeftVersionId, int nRightVersionId, ModelDiffGroups &groups);
        ModelComment addComment(int nDefineFileId, const std::string &sComment);
        void hideComment(int nCommentId);

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

