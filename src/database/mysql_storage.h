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
        ModelFile insertFile(const std::string &sGroup);

    private:
        std::string prepareStringValue(const std::string &sValue);
        int paramtoInt(const char *);
        std::string TAG;
        MYSQL *m_pConnection;
        std::mutex m_mtxConn;
        long m_nCreated;
};

class MySqlStorageUpdate {
    public:
        MySqlStorageUpdate(int nVersion);
        int getVersion();
        virtual bool apply(MySqlStorageConnection *) = 0;

    private:
        int m_nVersion;
};

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
        const std::vector<ModelGroup> &getGroupsAll();
        std::vector<ModelGroupForVersion> getGroups(int nVersionId);
        std::vector<ModelFile> getFiles(int nVersionId, int nGroupId, int nParentId);
        void getDiff(int nLeftVersionId, int nRightVersionId, ModelDiffGroups &groups);
        ModelComment addComment(int nDefineFileId, const std::string &sComment);
        void hideComment(int nCommentId);
        void addFiles(const std::vector<ModelFile *> &vFiles);

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

        // updates
        std::vector<MySqlStorageUpdate *> m_vDatabaseUpdates;
};

class MySqlStorageUpdate_0001 : public MySqlStorageUpdate {
    public:
        MySqlStorageUpdate_0001();
        virtual bool apply(MySqlStorageConnection *) override;
    private:
        std::string TAG;
};

class MySqlStorageUpdate_0002 : public MySqlStorageUpdate {
    public:
        MySqlStorageUpdate_0002();
        virtual bool apply(MySqlStorageConnection *) override;
    private:
        std::string TAG;
};

class MySqlStorageUpdate_0003 : public MySqlStorageUpdate {
    public:
        MySqlStorageUpdate_0003();
        virtual bool apply(MySqlStorageConnection *) override;
    private:
        std::string TAG;
};

class MySqlStorageUpdate_0004 : public MySqlStorageUpdate {
    public:
        MySqlStorageUpdate_0004();
        virtual bool apply(MySqlStorageConnection *) override;
    private:
        std::string TAG;
};

class MySqlStorageUpdate_0005 : public MySqlStorageUpdate {
    public:
        MySqlStorageUpdate_0005();
        virtual bool apply(MySqlStorageConnection *) override;
    private:
        std::string TAG;
};

class MySqlStorageUpdate_0006 : public MySqlStorageUpdate {
    public:
        MySqlStorageUpdate_0006();
        virtual bool apply(MySqlStorageConnection *) override;
    private:
        std::string TAG;
};

class MySqlStorageUpdate_0007 : public MySqlStorageUpdate {
    public:
        MySqlStorageUpdate_0007();
        virtual bool apply(MySqlStorageConnection *) override;
    private:
        std::string TAG;
};

class MySqlStorageUpdate_0008 : public MySqlStorageUpdate {
    public:
        MySqlStorageUpdate_0008();
        virtual bool apply(MySqlStorageConnection *) override;
    private:
        std::string TAG;
};

class MySqlStorageUpdate_0009 : public MySqlStorageUpdate {
    public:
        MySqlStorageUpdate_0009();
        virtual bool apply(MySqlStorageConnection *) override;
    private:
        std::string TAG;
};

#endif // MYSQL_STORAGE_H

