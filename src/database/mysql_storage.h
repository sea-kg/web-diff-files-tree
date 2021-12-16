#ifndef MYSQL_STORAGE_H
#define MYSQL_STORAGE_H

#include <map>
#include <mysql/mysql.h>
#include <mutex>
#include <vector>
#include "models_basic.h"
#include "models_diff.h"
#include "models_cache.h"
#include "mysql_storage_connection.h"
#include "mysql_storage_updates.h"

class MySqlStorage {
    public:
        MySqlStorage();
        MySqlStorageConnection * getConnection();
        void upgrade();
        bool loadCache();
        const std::vector<ModelVersion> &getVersionsAll();
        ModelVersion getVersionInfo(int nVersionId);
        ModelVersion findVersionOrCreate(const std::string &sVersion);
        ModelGroup findGroupOrCreate(const std::string &sGroup);
        int findDefineFileIdOrCreate(const std::string &sFilePath, const std::string &sFileName, int nDefineFileId);
        int findFileOrCreate(int nVersionId, int nGroupId, int nDefineFileId, int nFileParentId);

        const std::vector<ModelGroup> &getGroupsAll();
        std::vector<ModelGroupForVersion> getGroups(int nVersionId);
        std::vector<ModelFile> getFiles(int nVersionId, int nGroupId, int nParentId);
        void getDiff(int nLeftVersionId, int nRightVersionId, ModelDiffGroups &groups);
        ModelComment addComment(int nDefineFileId, const std::string &sComment);
        void hideComment(int nCommentId);
        void addFile(
            int nGroupId,
            int nVersionId,
            const std::string &sFilePath,
            int nFileSize,
            int nCompressSize,
            const std::string &sMode,
            bool bIsDir,
            const std::string &sDateTime
        );

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
        std::unordered_map<std::string, int> m_mapCacheFileDefinsIds;
        std::unordered_map<std::string, int> m_mapToUpdateFiles;
        ModelCacheFilesVersion m_cacheGroups;
        
        // updates
        MySqlStorageAllUpdates m_databaseUpdates;
};

#endif // MYSQL_STORAGE_H