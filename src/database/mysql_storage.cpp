#include "mysql_storage.h"
#include <mysql/mysql.h>
#include <wsjcpp_core.h>
#include <sstream>

MySqlStorageConnection::MySqlStorageConnection(MYSQL *pConn, MySqlStorage *pStorage) {
    m_pConnection = pConn;
    TAG = "MySqlStorageConenction";
    m_pStorage = pStorage;
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

std::string MySqlStorageConnection::lastDatabaseVersion() {
    std::lock_guard<std::mutex> lock(m_mtxConn);

    std::string sLastVersion = "";
    std::string sQuery = "SELECT version FROM updates ORDER BY id DESC LIMIT 0,1";

    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        if (sError.find("updates' doesn't exist") != std::string::npos) {
            WsjcppLog::warn(TAG, "Creating table updates .... ");
            std::string sTableDbUpdates = 
                "CREATE TABLE IF NOT EXISTS updates ("
                "  id INT NOT NULL AUTO_INCREMENT,"
                "  version varchar(255) DEFAULT NULL,"
                "  description text,"
                "  datetime_update datetime DEFAULT NULL,"
                "  PRIMARY KEY (`id`)"
                ") ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;";
            if (mysql_query(m_pConnection, sTableDbUpdates.c_str())) {
                std::string sError2(mysql_error(m_pConnection));
                WsjcppLog::err(TAG, "Problem on create table updates " + sError2);
                return "error";
            } else {
                WsjcppLog::ok(TAG, "Table updates success created");
                sLastVersion = "";
                return "";
            }
        } else {
            WsjcppLog::err(TAG, "Problem with database " + sError);
            return "error";
        }
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        if ((row = mysql_fetch_row(pRes)) != NULL) {
            sLastVersion = std::string(row[0]);
        }
        mysql_free_result(pRes);
    }
    return sLastVersion;
}

// ----------------------------------------------------------------------

std::vector<std::string> MySqlStorageConnection::getInstalledVersions() {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::vector<std::string> vVersions;

    std::string sQuery = "SELECT version FROM updates ORDER BY id";

    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        if (sError.find("updates' doesn't exist") != std::string::npos) {
            WsjcppLog::warn(TAG, "Creating table updates .... ");
            std::string sTableDbUpdates = 
                "CREATE TABLE IF NOT EXISTS updates ("
                "  id INT NOT NULL AUTO_INCREMENT,"
                "  version varchar(255) DEFAULT NULL,"
                "  description text,"
                "  datetime_update datetime DEFAULT NULL,"
                "  PRIMARY KEY (`id`)"
                ") ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;";
            if (mysql_query(m_pConnection, sTableDbUpdates.c_str())) {
                std::string sError2(mysql_error(m_pConnection));
                WsjcppLog::throw_err(TAG, "Problem on create table updates " + sError2);
            } else {
                WsjcppLog::ok(TAG, "Table updates success created");
                return vVersions;
            }
        } else {
            WsjcppLog::throw_err(TAG, "Problem with database " + sError);
        }
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            vVersions.push_back(std::string(row[0]));
        }
        mysql_free_result(pRes);
    }
    return vVersions;
}

// ----------------------------------------------------------------------

std::vector<std::string> MySqlStorageConnection::getApiVersionsAll() {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::vector<std::string> vVersions;

    std::string sQuery = "SELECT id, `name` FROM webdiff_versions ORDER BY `name`";

    if (mysql_query(m_pConnection, sQuery.c_str())) {
        std::string sError(mysql_error(m_pConnection));
        WsjcppLog::throw_err(TAG, "Problem with database " + sError);
    } else {
        MYSQL_RES *pRes = mysql_use_result(m_pConnection);
        MYSQL_ROW row;
        // output table name
        while ((row = mysql_fetch_row(pRes)) != NULL) {
            vVersions.push_back(std::string(row[0]));
        }
        mysql_free_result(pRes);
    }
    return vVersions;
}

// ----------------------------------------------------------------------

bool MySqlStorageConnection::insertUpdateInfo(const std::string &sVersion, const std::string &sDescription) {
    std::lock_guard<std::mutex> lock(m_mtxConn);
    std::string sInsertNewVersion = "INSERT INTO updates(version, description, datetime_update) "
        " VALUES(" + m_pStorage->prepareStringValue(sVersion) + ", " + m_pStorage->prepareStringValue(sDescription) + ",NOW());";
    if (mysql_query(m_pConnection, sInsertNewVersion.c_str())) {
        WsjcppLog::err(TAG, "Could not insert row to updates: " + std::string(mysql_error(m_pConnection)));
        return false;
    }
    return true;
}

// ----------------------------------------------------------------------
// MySqlStorage

MySqlStorage::MySqlStorage() {
    TAG = "MySqlStorage";
    
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
        pConn = new MySqlStorageConnection(pDatabase, this);
    }
    return pConn;
}

std::string MySqlStorage::prepareStringValue(const std::string &sValue) {
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
