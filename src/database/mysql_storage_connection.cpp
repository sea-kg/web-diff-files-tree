#include "mysql_storage.h"
#include <mysql/mysql.h>
#include <wsjcpp_core.h>
#include <sstream>
#include "mysql_storage_updates.h"

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
        "    t1.filename, "
        "    t0.is_dir, "
        "    t0.f_size, "
        "    t0.f_compress_size, "
        "    t0.dt, "
        "    t0.f_mode "
        "FROM webdiff_files t0 "
        "INNER JOIN webdiff_define_files t1 ON t1.id = t0.define_file_id "
        "INNER JOIN webdiff_file_groups t2 ON t2.id = t0.file_group_id "
        "LEFT JOIN webdiff_files t3 ON t0.define_file_id = t3.define_file_id AND t3.version_id = " + std::to_string(nRightVersionId) + " AND t0.file_group_id = t3.file_group_id "
        "WHERE t0.version_id = " + std::to_string(nLeftVersionId) + " AND t3.id is null"
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
            model.setIsDir(paramtoInt(row[8]));

            if (sState == "new") {
                model.setRightFileSize(paramtoInt(row[9]));
                model.setRightFileCompressedSize(paramtoInt(row[10]));
                model.setRightFileDateTime(std::string(row[11]));
                model.setRightFileMode(std::string(row[12]));
            } else if (sState == "missing") {
                model.setLeftFileSize(paramtoInt(row[9]));
                model.setLeftFileCompressedSize(paramtoInt(row[10]));
                model.setLeftFileDateTime(std::string(row[11]));
                model.setLeftFileMode(std::string(row[12]));
            }
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

ModelVersion MySqlStorageConnection::insertVersion(const std::string &sVersion) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    ModelVersion ver;
    std::string sQuery =
        " INSERT INTO webdiff_versions(`name`, comment)"
        " VALUES(" + this->prepareStringValue(sVersion) + ",'');";
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
        ver.setId(nLastInsertedId);
        ver.setName(sVersion);
    }
    return ver;
}

ModelGroup MySqlStorageConnection::insertGroup(const std::string &sGroup) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    ModelGroup gr;
    std::string sQuery =
        " INSERT INTO webdiff_file_groups(`name`)"
        " VALUES(" + this->prepareStringValue(sGroup) + ");";
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
        gr.setId(nLastInsertedId);
        gr.setTitle(sGroup);
    }
    return gr;
}

int MySqlStorageConnection::insertDefineFile(
    const std::string &sFilepath,
    const std::string &sFilename,
    int nParentId
) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    int nRet = -1;
    std::string sQuery =
        " INSERT INTO webdiff_define_files(filepath, `filename`, parent_id)"
        " VALUES(" + this->prepareStringValue(sFilepath) + ", " + this->prepareStringValue(sFilename) + ", " + std::to_string(nParentId) + ");";
    ;
    // WsjcppLog::info(TAG, sQuery);
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        // mysql_insert_id()
        nRet = mysql_insert_id(m_pConnection);
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        mysql_free_result(pRes);
        // gr.setId(nLastInsertedId);
        // gr.setTitle(sGroup);
    }
    return nRet;
}

void MySqlStorageConnection::getDefineFilesCache(std::unordered_map<std::string, int> &mapCache) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::string sQuery =
        " SELECT id, filepath FROM webdiff_define_files;"
    ;
    // WsjcppLog::info(TAG, sQuery);
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        int nCount = 0;
        // output table name
        WsjcppLog::info(TAG, "Loading cache define files ids...");
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            int nDefineFileId = paramtoInt(row[0]);
            std::string sFilePath = std::string(row[1]);
            mapCache[sFilePath] = nDefineFileId;
            nCount++;
        }
        WsjcppLog::info(TAG, "Loaded " + std::to_string(nCount) + " records");
        mysql_free_result(pRes);
    }
}

void MySqlStorageConnection::loadFilesIds(ModelCacheFilesVersion &cache) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::string sQuery =
        " SELECT id, version_id, define_file_id, file_group_id FROM webdiff_files;"
    ;
    // WsjcppLog::info(TAG, sQuery);
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        int nCount = 0;
        // output table name
        WsjcppLog::info(TAG, "Loading cache files ids...");
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            int nFileId = paramtoInt(row[0]);
            int nVersionId = paramtoInt(row[1]);
            int nDefineFileId = paramtoInt(row[2]);
            int nGroupId = paramtoInt(row[3]);
            cache.add(nVersionId, nGroupId, nDefineFileId, nFileId);
            nCount++;
        }
        WsjcppLog::info(TAG, "Loaded " + std::to_string(nCount) + " records");
        mysql_free_result(pRes);
    }
}

int MySqlStorageConnection::insertFile(int nVersionId, int nGroupId, int nDefineFileId, int nFileParentId) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    int nRet = -1;
    std::string sQuery =
        " INSERT INTO webdiff_files(version_id, file_group_id, define_file_id,  parent_id)"
        " VALUES(" + std::to_string(nVersionId) + "," + std::to_string(nGroupId) + "," + std::to_string(nDefineFileId) + "," + std::to_string(nFileParentId) + ");"
    ;
    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        nRet = mysql_insert_id(m_pConnection);
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        mysql_free_result(pRes);
    }

    // update amnount of childs
    int nAmountOfChildren = 0;
    std::string sQuery2 =
        " SELECT COUNT(id) as cnt FROM webdiff_files WHERE parent_id = " + std::to_string(nFileParentId) + ";"
    ;
    if (mysql_query(m_pConnection, sQuery2.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        if ((row = mysql_fetch_row(pRes)) != NULL) {
            nAmountOfChildren = paramtoInt(row[0]);
        }
        mysql_free_result(pRes);
    }

    std::string sQuery3 =
        " UPDATE webdiff_files SET amount_of_children = " + std::to_string(nAmountOfChildren) + " WHERE id = " + std::to_string(nFileParentId) + ";"
    ;
    if (mysql_query(m_pConnection, sQuery3.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        mysql_free_result(pRes);
    }
    return nRet;
}
