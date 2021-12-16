#include "mysql_storage.h"
#include <mysql/mysql.h>
#include <wsjcpp_core.h>
#include <sstream>
#include "mysql_storage_updates.h"

// ----------------------------------------------------------------------
// MySqlStorage

MySqlStorage::MySqlStorage() {
    TAG = "MySqlStorage";
    m_nConnectionOutdatedAfterSeconds = 60*60; // 1 hour

    if (!WsjcppCore::getEnv("WEBDIFF_DB_HOST", m_sDatabaseHost)) {
        m_sDatabaseHost = "localhost";
    }

    if (!WsjcppCore::getEnv("WEBDIFF_DB_NAME", m_sDatabaseName)) {
        m_sDatabaseName = "webdiff";
    }

    if (!WsjcppCore::getEnv("WEBDIFF_DB_USER", m_sDatabaseUser)) {
        m_sDatabaseUser = "webdiffu";
    }

    if (!WsjcppCore::getEnv("WEBDIFF_DB_PASS", m_sDatabasePass)) {
        m_sDatabasePass = "jET3E4W9vm";
    }

    std::string sDatabasePort;
    if (WsjcppCore::getEnv("WEBDIFF_DB_PORT", sDatabasePort)) {
        std::stringstream intValue(sDatabasePort);
        int number = 0;
        intValue >> number;
    } else {
        m_nDatabasePort = 3306;
    }
    
    WsjcppLog::info(TAG, "Database host: " + m_sDatabaseHost);
    WsjcppLog::info(TAG, "Database port: " + std::to_string(m_nDatabasePort));
    WsjcppLog::info(TAG, "Database name: " + m_sDatabaseName);
    WsjcppLog::info(TAG, "Database user: " + m_sDatabaseUser);
    WsjcppLog::info(TAG, "Database password: (hided)");
}

MySqlStorageConnection * MySqlStorage::connect() {
    MySqlStorageConnection *pConn = nullptr;
    MYSQL *pDatabase = mysql_init(NULL);
    if (!mysql_real_connect(pDatabase, 
            m_sDatabaseHost.c_str(),
            m_sDatabaseUser.c_str(),
            m_sDatabasePass.c_str(),
            m_sDatabaseName.c_str(), 
            m_nDatabasePort, NULL, 0)) {
        WsjcppLog::err(TAG, "Connect error: " + std::string(mysql_error(pDatabase)));
        WsjcppLog::err(TAG, "Failed to connect.");
    } else {
        pConn = new MySqlStorageConnection(pDatabase);
    }
    return pConn;
}


MySqlStorageConnection * MySqlStorage::getConnection() {
    std::lock_guard<std::mutex> lock(m_mtxStorageConnections);
    
    if (m_vDoRemoveStorageConnections.size() > 0) {
        WsjcppLog::warn(TAG, "TODO cleanup m_vDoRemoveStorageConnections, size = " + std::to_string(m_vDoRemoveStorageConnections.size()));
    }

    std::string sThreadId = WsjcppCore::getThreadId();
    MySqlStorageConnection *pStorageConnection = nullptr;
    std::map<std::string, MySqlStorageConnection *>::iterator it;
    it = m_mapStorageConnections.find(sThreadId);
    if (it == m_mapStorageConnections.end()) {
        pStorageConnection = this->connect();
        if (pStorageConnection == nullptr) {
            return nullptr;
        }
        m_mapStorageConnections[sThreadId] = pStorageConnection;
    } else {
        pStorageConnection = it->second;
        // if connection outdated just reconnect this also maybe need keep several time last connection
        if (pStorageConnection->getConnectionDurationInSeconds() > m_nConnectionOutdatedAfterSeconds) {
            m_vDoRemoveStorageConnections.push_back(pStorageConnection);
            pStorageConnection = this->connect();
            if (pStorageConnection == nullptr) {
                return nullptr;
            }
            m_mapStorageConnections[sThreadId] = pStorageConnection;
        }
    }
    return pStorageConnection;
}

void MySqlStorage::upgrade() {
    MySqlStorageConnection *pConn = getConnection();
    m_databaseUpdates.upgrade(pConn);
}

bool MySqlStorage::loadCache() {
    MySqlStorageConnection *pConn = getConnection();
    m_vVersions = pConn->getApiVersionsAll();
    m_vGroups = pConn->getGroupsAll();
    pConn->getDefineFilesCache(m_mapCacheFileDefinsIds);
    pConn->loadFilesIds(m_cacheGroups);
    return true;
}

const std::vector<ModelVersion> &MySqlStorage::getVersionsAll() {
    // from cache
    return m_vVersions;
}

ModelVersion MySqlStorage::getVersionInfo(int nVersionId) {
    ModelVersion ret;
    for (int i = 0; i < m_vVersions.size(); i++) {
        if (m_vVersions[i].getId() == nVersionId) {
            ret = m_vVersions[i];
        }
    }
    return ret;
}

ModelVersion MySqlStorage::findVersionOrCreate(const std::string &sVersion) {
    // TODO mutex
    ModelVersion ret;
    bool bFound = false;
    for (int i = 0; i < m_vVersions.size(); i++) {
        if (m_vVersions[i].getName() == sVersion) {
            ret = m_vVersions[i];
            bFound = true;
            break;
        }
    }
    if (!bFound) {
        MySqlStorageConnection *pConn = getConnection();
        ret = pConn->insertVersion(sVersion);
        m_vVersions.push_back(ret);
    }
    return ret;
}

ModelGroup MySqlStorage::findGroupOrCreate(const std::string &sGroup) {
    // TODO mutex
    ModelGroup ret;
    bool bFound = false;
    for (int i = 0; i < m_vGroups.size(); i++) {
        if (m_vGroups[i].getTitle() == sGroup) {
            ret = m_vGroups[i];
            bFound = true;
            break;
        }
    }
    if (!bFound) {
        MySqlStorageConnection *pConn = getConnection();
        ret = pConn->insertGroup(sGroup);
        m_vGroups.push_back(ret);
    }
    return ret;
}

int MySqlStorage::findDefineFileIdOrCreate(const std::string &sFilePath, const std::string &sFileName, int nDefineFileId) {
    int nNewDefineFileId = -1;
    std::unordered_map<std::string, int>::iterator it;
    it = m_mapCacheFileDefinsIds.find(sFilePath);
    if (it == m_mapCacheFileDefinsIds.end()) {
        MySqlStorageConnection *pConn = getConnection();
        nNewDefineFileId = pConn->insertDefineFile(sFilePath, sFileName, nDefineFileId);
        m_mapCacheFileDefinsIds[sFilePath] = nNewDefineFileId;
    } else {
        nNewDefineFileId = it->second;
    }
    return nNewDefineFileId;
}

int MySqlStorage::findFileOrCreate(int nVersionId, int nGroupId, int nDefineFileId, int nFileParentId) {
    MySqlStorageConnection *pConn = getConnection();
    int nFileId = m_cacheGroups.findFileId(nVersionId, nGroupId, nDefineFileId);
    if (nFileId != -1) {
        return nFileId;
    }
    nFileId = pConn->insertFile(nVersionId, nGroupId, nDefineFileId, nFileParentId);
    m_cacheGroups.add(nVersionId, nGroupId, nDefineFileId, nFileId);
    return nFileId;
}

const std::vector<ModelGroup> &MySqlStorage::getGroupsAll() {
    // from cache
    return m_vGroups;
}

std::vector<ModelGroupForVersion> MySqlStorage::getGroups(int nVersionId) {
    MySqlStorageConnection *pConn = getConnection();
    return pConn->getGroups(nVersionId);
}

std::vector<ModelFile> MySqlStorage::getFiles(int nVersionId, int nGroupId, int nParentId) {
    MySqlStorageConnection *pConn = getConnection();
    return pConn->getFiles(nVersionId, nGroupId, nParentId);
}

void MySqlStorage::getDiff(int nLeftVersionId, int nRightVersionId, ModelDiffGroups &diffGroups) {
    MySqlStorageConnection *pConn = getConnection();
    long nStartTime = WsjcppCore::getCurrentTimeInMilliseconds();
    WsjcppLog::info(TAG, "Search missing...");
    pConn->getDiffFiles(nLeftVersionId, nRightVersionId, "missing", diffGroups);
    WsjcppLog::info(TAG, "Done (" + std::to_string(diffGroups.getFilesLength()) + " files");
    WsjcppLog::info(TAG, "Search new...");
    pConn->getDiffFiles(nRightVersionId, nLeftVersionId, "new", diffGroups);
    WsjcppLog::info(TAG, "Done (" + std::to_string(diffGroups.getFilesLength()) + " files");

    WsjcppLog::info(TAG, "Search parents...");
    const std::map<int, ModelDiffGroup> &groups = diffGroups.getGroups();
    for (auto it = groups.begin(); it != groups.end(); ++it) {
        const ModelGroup &group = it->second.getGroup();
        std::vector<int> vIds = it->second.getParentFilesIds();
        while (vIds.size() > 0) {
            int nFileId = vIds.back();
            vIds.pop_back();
            if (nFileId == 0 || it->second.hasFileId(nFileId)) {
                continue;
            }
            pConn->findAndAddFile(group, nFileId, diffGroups, vIds);
        }
    }
    WsjcppLog::info(TAG, "Done (" + std::to_string(diffGroups.getFilesLength()) + " files");

    WsjcppLog::info(TAG, "Search comments...");
    const std::vector<int> &vDefineFilesIds = diffGroups.getFilesDefinesIds();
    for (int i = 0; i < vDefineFilesIds.size(); i++) {
        int nDefineFileId = vDefineFilesIds[i];
        diffGroups.setComments(nDefineFileId, pConn->getComments(nDefineFileId));
    }
    WsjcppLog::info(TAG, "Done (" + std::to_string(diffGroups.getCommentsLength()) + " comments");
    long nElapsedTime = WsjcppCore::getCurrentTimeInMilliseconds() - nStartTime;
    WsjcppLog::info(TAG, "Elapsed " + std::to_string(nElapsedTime) + " ms");
}

ModelComment MySqlStorage::addComment(int nDefineFileId, const std::string &sComment) {
    MySqlStorageConnection *pConn = getConnection();
    return pConn->addComment(nDefineFileId, sComment);
}

void MySqlStorage::hideComment(int nCommentId) {
    MySqlStorageConnection *pConn = getConnection();
    pConn->hideComment(nCommentId);
}

void MySqlStorage::addFile(
    int nVersionId,
    int nGroupId,
    const std::string &sFilePath,
    int nFileSize,
    int nCompressSize,
    const std::string &sMode,
    bool bIsDir,
    const std::string &sDateTime
) {
    if (sFilePath == "" || sFilePath == "/") {
        return;
    }
    std::string sPath = WsjcppCore::doNormalizePath(sFilePath);
    std::vector<std::string> vPath = WsjcppCore::split(sPath, "/");
    int nDefineFileId = 0;
    int nFileParentId = 0;

    std::string sNewFilePath = "";
    for (int i = 0; i < vPath.size(); i++) {
        if (vPath[i] == "") {
            continue;
        }
        sNewFilePath += "/" + vPath[i];
        nDefineFileId = this->findDefineFileIdOrCreate(sNewFilePath, vPath[i], nDefineFileId);
        nFileParentId = this->findFileOrCreate(nVersionId, nGroupId, nDefineFileId, nFileParentId);
        // m_mapToUpdateFiles[nFileParentId] - recalculate size, and etc 
    }
    // nFileParentId
    // TODO update compress size and etc
}
