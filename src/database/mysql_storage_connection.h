#ifndef MYSQL_STORAGE_CONNECTION_H
#define MYSQL_STORAGE_CONNECTION_H

#include <map>
#include <mysql/mysql.h>
#include <mutex>
#include <vector>
#include "models_basic.h"
#include "models_diff.h"
#include "models_cache.h"

class MySqlStorage;

class MySqlStorageConnection {
    friend MySqlStorage;
    public:
        MySqlStorageConnection(MYSQL *pConn);
        ~MySqlStorageConnection();
        bool executeQuery(const std::string &sQuery);
        int lastDatabaseVersion();
        bool insertUpdateInfo(int nVersion);

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
        ModelVersion insertVersion(const std::string &sVersion);
        ModelGroup insertGroup(const std::string &sGroup);
        int insertDefineFile(
            const std::string &sFilepath,
            const std::string &sFilename,
            int nParentId
        );
        
        void getDefineFilesCache(std::unordered_map<std::string, int> &mapCache);
        void loadFilesIds(ModelCacheFilesVersion &cache);
        int insertFile(int nVersionId, int nGroupId, int nDefineFileId, int nFileParentId);

    private:
        std::string prepareStringValue(const std::string &sValue);
        int paramtoInt(const char *);
        std::string TAG;
        MYSQL *m_pConnection;
        std::mutex m_mtxConn;
        long m_nCreated;
};

#endif // MYSQL_STORAGE_CONNECTION_H