#include "mysql_storage.h"
#include <mysql/mysql.h>
#include <wsjcpp_core.h>
#include <sstream>

MySqlStorageConnection::MySqlStorageConnection(MYSQL *pConn) {
    m_pConnection = pConn;
    TAG = "MySqlStorageConnection";
    m_nCreated = WsjcppCore::getCurrentTimeInMilliseconds();
}

// ----------------------------------------------------------------------

MySqlStorageConnection::~MySqlStorageConnection() {
    mysql_close(m_pConnection);
    // delete m_pConnection;
}

// ----------------------------------------------------------------------

bool MySqlStorageConnection::executeQuery(const std::string &sQuery) {
    // TODO statistics time
    std::lock_guard<std::mutex> lock(m_mtxConn);
    // WsjcppLog::info(TAG, "Try " + sQuery);
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        WsjcppLog::err(TAG, "Problem on executeQuery \r\nQuery: " + sQuery);
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::err(TAG, "executeQuery error " + sError);
        return false;
    } else {
        // WsjcppLog::ok(TAG, "" + sQuery);
    }
    return true;
}

// ----------------------------------------------------------------------

int MySqlStorageConnection::lastDatabaseVersion() {
    std::lock_guard<std::mutex> lock(m_mtxConn);

    int nLastVersion = 0;
    std::string sQuery = "SELECT version FROM webdiff_db_updates ORDER BY dt DESC, version DESC LIMIT 0,1";

    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        if (sError.find("doesn't exist") != std::string::npos) {
            WsjcppLog::warn(TAG, "Creating table webdiff_db_updates .... ");
            std::string sTableDbUpdates = 
                " CREATE TABLE webdiff_db_updates ("
                "     `id` int(11) NOT NULL AUTO_INCREMENT,"
                "     `version` int(11) NOT NULL,"
                "     `dt` datetime NOT NULL,"
                "     PRIMARY KEY (`id`)"
                " ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;"
            ;
            if (mysql_query(m_pConnection, sTableDbUpdates.c_str())) {
                std::string sError2(mysql_error(m_pConnection));
                WsjcppLog::err(TAG, "Problem on create table webdiff_db_updates " + sError2);
                return -2;
            } else {
                WsjcppLog::ok(TAG, "Table webdiff_db_updates success created");
                return nLastVersion;
            }
        } else {
            WsjcppLog::err(TAG, "Problem with database " + sError);
            return -1;
        }
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        if ((row = mysql_fetch_row(pRes)) != NULL) {
            nLastVersion = paramtoInt(row[0]);
        }
        mysql_free_result(pRes);
    }
    return nLastVersion;
}

bool MySqlStorageConnection::insertUpdateInfo(int nVersion) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::string sInsertNewVersion = "INSERT INTO webdiff_db_updates(version, dt) "
        " VALUES(" + std::to_string(nVersion) + ", NOW());";
    if (mysql_query(m_pConnection, sInsertNewVersion.c_str())) {
        WsjcppLog::err(TAG, "Could not insert row to updates: " + std::string(mysql_error(m_pConnection)));
        return false;
    }
    return true;
}

long MySqlStorageConnection::getConnectionDurationInSeconds() {
    long nRet = WsjcppCore::getCurrentTimeInMilliseconds() - m_nCreated;
    nRet = nRet / 1000;
    return nRet;
}

std::string MySqlStorageConnection::prepareStringValue(const std::string &sValue) {
    // escaping simbols  NUL (ASCII 0), \n, \r, \, ', ", и Control-Z.
    std::string sResult;
    sResult.reserve(sValue.size()*2);
    sResult.push_back('"');
    for (int i = 0; i < sValue.size(); i++) {
        char c = sValue[i];
        if (c == '\n') {
            sResult.push_back('\\');
            sResult.push_back('n');
        } else if (c == '\r') {
            sResult.push_back('\\');
            sResult.push_back('r');
        } else if (c == '\\' || c == '"' || c == '\'') {
            sResult.push_back('\\');
            sResult.push_back(c);
        } else if (c == 0) {
            sResult.push_back('\\');
            sResult.push_back('0');
        } else {
            sResult.push_back(c);
        }
    }
    sResult.push_back('"');
    return sResult;
}

int MySqlStorageConnection::paramtoInt(const char *p) {
    std::stringstream intValue(p);
    int number = 0;
    intValue >> number;
    return number;
}

std::vector<ModelVersion> MySqlStorageConnection::getApiVersionsAll() {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::vector<ModelVersion> vRet;

    std::string sQuery = "SELECT id, `name` FROM webdiff_versions ORDER BY `name`";

    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            ModelVersion model;
            model.setId(paramtoInt(row[0]));
            model.setName(std::string(row[1]));
            vRet.push_back(model);
        }
        mysql_free_result(pRes);
    }
    return vRet;
}

std::vector<ModelGroup> MySqlStorageConnection::getGroupsAll() {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::vector<ModelGroup> vRet;

    std::string sQuery = "SELECT id, `name` FROM webdiff_file_groups ORDER BY `name`";

    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            ModelGroup model;
            model.setId(paramtoInt(row[0]));
            model.setTitle(std::string(row[1]));
            vRet.push_back(model);
        }
        mysql_free_result(pRes);
    }
    return vRet;
}

std::vector<ModelGroupForVersion> MySqlStorageConnection::getGroups(int nVersionId) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::vector<ModelGroupForVersion> vRet;

    std::string sQuery =
        " SELECT"
        "   version_id,"
        "   file_group_id,"
        "   `name` as title,"
        "   COUNT(*) as amount_of_files"
        " FROM webdiff_files"
        " INNER JOIN webdiff_file_groups t1 ON t1.id = file_group_id"
        " WHERE version_id = " + std::to_string(nVersionId) + 
        " GROUP BY version_id, file_group_id"
    ;
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            ModelGroupForVersion model;
            model.setVersionId(paramtoInt(row[0]));
            model.setId(paramtoInt(row[1]));
            model.setTitle(std::string(row[2]));
            model.setAmountOfFiles(paramtoInt(row[3]));
            vRet.push_back(model);
        }
        mysql_free_result(pRes);
    }
    return vRet;
}

std::vector<ModelComment> MySqlStorageConnection::getComments(int nDefineFileId) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::vector<ModelComment> vRet;

    std::string sQuery =
        " SELECT"
        "   t0.id,"
        "   t0.define_file_id,"
        "   t0.comment"
        " FROM webdiff_define_files_comments t0"
        " WHERE t0.define_file_id = " + std::to_string(nDefineFileId) + " AND hided = 0"
        " ORDER BY dt_created DESC"
    ;
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            ModelComment model;
            model.setId(paramtoInt(row[0]));
            model.setDefineFileId(paramtoInt(row[1]));
            model.setComment(std::string(row[2]));
            vRet.push_back(model);
        }
        mysql_free_result(pRes);
    }
    return vRet;
}

std::vector<ModelFile> MySqlStorageConnection::getFiles(int nVersionId, int nGroupId, int nParentId) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::vector<ModelFile> vRet;
    std::string sQuery =
        " SELECT"
        "   t0.version_id,"
        "   t0.file_group_id,"
        "   t0.id as file_id,"
        "   t0.amount_of_children,"
//        "   t1.filepath,"
        "   t1.filename as title"
        " FROM webdiff_files t0"
        " INNER JOIN webdiff_define_files t1 ON t1.id = define_file_id"
        " WHERE t0.version_id = " + std::to_string(nVersionId) +
            " AND file_group_id = " + std::to_string(nGroupId) + 
            " AND t0.parent_id = " + std::to_string(nParentId) +
        " ORDER BY t0.amount_of_children <> 0 DESC, title"
    ;
    // WsjcppLog::info(TAG, sQuery);
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            ModelFile model;
            model.setVersionId(paramtoInt(row[0]));
            model.setGroupId(paramtoInt(row[1]));
            model.setId(paramtoInt(row[2]));
            model.setAmountOfChildren(paramtoInt(row[3]));
            model.setTitle(std::string(row[4]));
            vRet.push_back(model);
        }
        mysql_free_result(pRes);
    }
    return vRet;
}

void MySqlStorageConnection::findAndAddFile(const ModelGroup &group, int nFileId, ModelDiffGroups &diffGroups, std::vector<int> &vParentFileIds) {
    // std::lock_guard<std::mutex> lock(m_mtxConn);
    std::string sQuery =
        " SELECT"
        "   t0.id,"
        "   t0.parent_id,"
        // "   t0.file_group_id,"
        "   t0.amount_of_children,"
        "   t0.define_file_id,"
        "   t1.filepath,"
        "   t1.filename"
        " FROM webdiff_files t0"
        " INNER JOIN webdiff_define_files t1 ON t1.id = t0.define_file_id"
        " WHERE t0.id = " + std::to_string(nFileId) + ";"
    ;
    // WsjcppLog::info(TAG, sQuery);
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            // std::cout << "file" << std::endl;
            ModelDiffFile model;
            int nFileId = paramtoInt(row[0]);
            vParentFileIds.push_back(nFileId);
            model.setId(nFileId);
            model.setParentFileId(paramtoInt(row[1]));
            model.setAmountOfChildren(paramtoInt(row[2]));
            model.setDefineFileId(paramtoInt(row[3]));
            model.setFilepath(std::string(row[4]));
            model.setFilename(std::string(row[5]));
            model.setState("");
            diffGroups.addDiffFile(group, model);
        }
        mysql_free_result(pRes);
    }
}

void MySqlStorageConnection::getDiffFiles(int nLeftVersionId, int nRightVersionId, const std::string &sState, ModelDiffGroups &diffGroups) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::string sQuery =
        "SELECT "
        "    t0.id, "
        "    t0.parent_id, "
        "    t0.file_group_id, "
        "    t0.amount_of_children, "
        "    t2.name as group_name, "
        "    t0.define_file_id, "
        "    t1.filepath, "
        "    t1.filename "
        "FROM webdiff_files t0 "
        "INNER JOIN webdiff_define_files t1 ON t1.id = t0.define_file_id "
        "INNER JOIN webdiff_file_groups t2 ON t2.id = t0.file_group_id "
        "WHERE "
        "  version_id = " + std::to_string(nLeftVersionId) + 
        "  AND define_file_id NOT IN (SELECT define_file_id FROM webdiff_files t10 WHERE version_id = " + std::to_string(nRightVersionId) + " AND t0.file_group_id = t10.file_group_id); "
    ;
    // WsjcppLog::info(TAG, sQuery);
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            // std::cout << "file" << std::endl;
            ModelDiffFile model;
            ModelGroup group;
            model.setId(paramtoInt(row[0]));
            model.setParentFileId(paramtoInt(row[1]));
            group.setId(paramtoInt(row[2]));
            model.setAmountOfChildren(paramtoInt(row[3]));
            group.setTitle(std::string(row[4]));
            model.setDefineFileId(paramtoInt(row[5]));
            model.setFilepath(std::string(row[6]));
            model.setFilename(std::string(row[7]));
            model.setState(sState);
            // TODO set comments list
            diffGroups.addDiffFile(group, model);
        }
        mysql_free_result(pRes);
    }
}

ModelComment MySqlStorageConnection::addComment(int nDefineFileId, const std::string &sComment) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    ModelComment comment;
    std::string sQuery =
        " INSERT INTO webdiff_define_files_comments(define_file_id, comment, dt_created)"
        " VALUES(" + std::to_string(nDefineFileId) + "," + this->prepareStringValue(sComment) + ",NOW());"   ;
    ;
    // WsjcppLog::info(TAG, sQuery);
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        // mysql_insert_id()
        int nLastInsertedId = mysql_insert_id(m_pConnection);
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        mysql_free_result(pRes);
        comment.setId(nLastInsertedId);
        comment.setDefineFileId(nDefineFileId);
        comment.setComment(sComment);
    }
    return comment;
}

void MySqlStorageConnection::hideComment(int nCommentId) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::string sQuery =
        "  UPDATE webdiff_define_files_comments SET hided = 1 WHERE id = " + std::to_string(nCommentId) + ";"
    ;
    // WsjcppLog::info(TAG, sQuery);
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        // nothing - it's ok
    }
}

// ----------------------------------------------------------------------
// MySqlStorageUpdate

MySqlStorageUpdate::MySqlStorageUpdate(int nVersion) {
    m_nVersion = nVersion;
}

int MySqlStorageUpdate::getVersion() {
    return m_nVersion;
}

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

    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0001());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0002());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0003());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0004());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0005());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0006());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0007());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0008());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0009());
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
    int nDbVersion = pConn->lastDatabaseVersion();
    if (nDbVersion < 0) {
        WsjcppLog::throw_err(TAG, "Could not get latest version of database");
    }
    WsjcppLog::info(TAG, "Last database version: " + std::to_string(nDbVersion));
    bool bFoundUpdate = true;
    while(bFoundUpdate) {
        bFoundUpdate = false;
        for (int i = 0; i < m_vDatabaseUpdates.size(); i++) {
            int nUpdateVersion = m_vDatabaseUpdates[i]->getVersion();
            if (nUpdateVersion - 1 == nDbVersion) {
                if (m_vDatabaseUpdates[i]->apply(pConn)) {
                    if (!pConn->insertUpdateInfo(nUpdateVersion)) {
                        WsjcppLog::throw_err(TAG, "Could not insert version");
                    }
                    bFoundUpdate = true;
                    nDbVersion = nUpdateVersion;
                    WsjcppLog::ok(TAG, "Applyed update " + std::to_string(nDbVersion));
                } else {
                    WsjcppLog::throw_err(TAG, "Could not apply update " + std::to_string(nDbVersion));
                }
            }
        }
    }
}

bool MySqlStorage::loadCache() {
    MySqlStorageConnection *pConn = getConnection();
    m_vVersions = pConn->getApiVersionsAll();
    m_vGroups = pConn->getGroupsAll();
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

    pConn->getDiffFiles(nLeftVersionId, nRightVersionId, "missing", diffGroups);
    pConn->getDiffFiles(nRightVersionId, nLeftVersionId, "new", diffGroups);

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

    const std::vector<int> &vDefineFilesIds = diffGroups.getFilesDefinesIds();
    for (int i = 0; i < vDefineFilesIds.size(); i++) {
        int nDefineFileId = vDefineFilesIds[i];
        diffGroups.setComments(nDefineFileId, pConn->getComments(nDefineFileId));
    }
}

ModelComment MySqlStorage::addComment(int nDefineFileId, const std::string &sComment) {
    MySqlStorageConnection *pConn = getConnection();
    return pConn->addComment(nDefineFileId, sComment);
}

void MySqlStorage::hideComment(int nCommentId) {
    MySqlStorageConnection *pConn = getConnection();
    pConn->hideComment(nCommentId);
}

// ----------------------------------------------------------------------
// MySqlStorageUpdate_0001

MySqlStorageUpdate_0001::MySqlStorageUpdate_0001() 
: MySqlStorageUpdate(1) {
    TAG = "MySqlStorageUpdate_0001";
}

bool MySqlStorageUpdate_0001::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " CREATE TABLE `webdiff_users` ("
        "     `id` int(11) NOT NULL AUTO_INCREMENT,"
        "     `username` varchar(255) NOT NULL,"
        "     `userpass` varchar(100) NOT NULL,"
        "     `role` varchar(10) NOT NULL,"
        "     `comment` varchar(1024) NOT NULL,"
        "     `created` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "     PRIMARY KEY (`id`),"
        "     UNIQUE(`username`)"
        " ) ENGINE=InnoDB DEFAULT CHARSET=utf8, AUTO_INCREMENT=1;"
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create table webdiff_users");
        return false;
    }

    bResult = pConn->executeQuery(
        " INSERT INTO webdiff_users(username,userpass,role,comment) VALUES(\"admin\",\"d033e22ae348aeb5660fc2140aec35850c4da997\",\"admin\",\"\") "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not insert into table webdiff_users");
        return false;
    }

    bResult = pConn->executeQuery(
        " CREATE TABLE `webdiff_users_tokens` ("
        "     `id` int(11) NOT NULL AUTO_INCREMENT,"
        "     `token` varchar(255) NOT NULL,"
        "     `role` varchar(10) NOT NULL,"
        "     `userid` int(11) NOT NULL,"
        "     `created` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "     PRIMARY KEY (`id`),"
        "     UNIQUE(`token`)"
        " ) ENGINE=InnoDB DEFAULT CHARSET=utf8, AUTO_INCREMENT=1;"
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create table webdiff_users_tokens");
        return false;
    }

    bResult = pConn->executeQuery(
        " CREATE TABLE `webdiff_versions` ("
        "     `id` int(11) NOT NULL AUTO_INCREMENT,"
        "     `name` varchar(255) NOT NULL,"
        "     `comment` varchar(255) NOT NULL,"
        "     PRIMARY KEY (`id`)"
        " ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;"
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create table webdiff_versions");
        return false;
    }
    return true;
}


// ----------------------------------------------------------------------
// MySqlStorageUpdate_0002

MySqlStorageUpdate_0002::MySqlStorageUpdate_0002() 
: MySqlStorageUpdate(2) {
    TAG = "MySqlStorageUpdate_0002";
}

bool MySqlStorageUpdate_0002::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " CREATE TABLE `webdiff_file_groups` ("
        "     `id` int(11) NOT NULL AUTO_INCREMENT,"
        "     `name` varchar(255) NOT NULL,"
        "     PRIMARY KEY (`id`)"
        " ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;"
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create table webdiff_file_groups");
        return false;
    }

    bResult = pConn->executeQuery(
        " CREATE TABLE `webdiff_define_files` ("
        "     `id` int(11) NOT NULL AUTO_INCREMENT,"
        "     `file_group_id` int(11) NOT NULL,"
        "     `filepath` varchar(4096) NOT NULL,"
        "     PRIMARY KEY (`id`)"
        " ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;"
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create table webdiff_define_files");
        return false;
    }

    bResult = pConn->executeQuery(
        " CREATE TABLE `webdiff_define_files_comments` ("
        "     `id` int(11) NOT NULL AUTO_INCREMENT,"
        "     `define_file_id` int(11) NOT NULL,"
        "     `comment` TEXT NOT NULL,"
        "     `dt_created` datetime NOT NULL,"
        "     PRIMARY KEY (`id`)"
        " ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;"
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create table webdiff_define_files_comments");
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------
// MySqlStorageUpdate_0003

MySqlStorageUpdate_0003::MySqlStorageUpdate_0003() 
: MySqlStorageUpdate(3) {
    TAG = "MySqlStorageUpdate_0003";
}

bool MySqlStorageUpdate_0003::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " CREATE TABLE `webdiff_files` ("
        "     `id` int(11) NOT NULL AUTO_INCREMENT,"
        "     `version_id` int(11) NOT NULL,"
        "     `define_file_id` int(11) NOT NULL,"
        "     PRIMARY KEY (`id`)"
        " ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;"
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create table webdiff_files");
        return false;
    }
    return true;
}

// ----------------------------------------------------------------------
// MySqlStorageUpdate_0004

MySqlStorageUpdate_0004::MySqlStorageUpdate_0004() 
: MySqlStorageUpdate(4) {
    TAG = "MySqlStorageUpdate_0004";
}

bool MySqlStorageUpdate_0004::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_define_files ADD childs int(11) DEFAULT 0; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_define_files (add childs)");
        return false;
    }
    
    bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_define_files ADD parent_id int(11) DEFAULT 0; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_define_files (add parent_id)");
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------
// MySqlStorageUpdate_0005

MySqlStorageUpdate_0005::MySqlStorageUpdate_0005() 
: MySqlStorageUpdate(5) {
    TAG = "MySqlStorageUpdate_0005";
}

bool MySqlStorageUpdate_0005::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_define_files ADD `filename` VARCHAR(255) DEFAULT ''; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_define_files (add filename)");
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------
// MySqlStorageUpdate_0006

MySqlStorageUpdate_0006::MySqlStorageUpdate_0006() 
: MySqlStorageUpdate(6) {
    TAG = "MySqlStorageUpdate_0006";
}

bool MySqlStorageUpdate_0006::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_files ADD file_group_id int(11) DEFAULT 0; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_files (add file_group_id)");
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------
// MySqlStorageUpdate_0007

MySqlStorageUpdate_0007::MySqlStorageUpdate_0007() 
: MySqlStorageUpdate(7) {
    TAG = "MySqlStorageUpdate_0007";
}

bool MySqlStorageUpdate_0007::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_define_files DROP COLUMN file_group_id; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_define_files (remove file_group_id)");
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------
// MySqlStorageUpdate_0008

MySqlStorageUpdate_0008::MySqlStorageUpdate_0008() 
: MySqlStorageUpdate(8) {
    TAG = "MySqlStorageUpdate_0008";
}

bool MySqlStorageUpdate_0008::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_files ADD parent_id int(11) DEFAULT 0; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_files (add parent_id)");
        return false;
    }

    bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_files ADD amount_of_children int(11) DEFAULT 0; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_files (add amount_of_children)");
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------
// MySqlStorageUpdate_0009

MySqlStorageUpdate_0009::MySqlStorageUpdate_0009() 
: MySqlStorageUpdate(9) {
    TAG = "MySqlStorageUpdate_0009";
}

bool MySqlStorageUpdate_0009::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_define_files_comments ADD hided int(11) DEFAULT 0; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_files (add parent_id)");
        return false;
    }
    return true;
}