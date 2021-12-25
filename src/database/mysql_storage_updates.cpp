#include "mysql_storage_updates.h"
#include <wsjcpp_core.h>
#include "mysql_storage.h"

// ----------------------------------------------------------------------
// MySqlStorageUpdate

MySqlStorageUpdate::MySqlStorageUpdate(int nVersion) {
    m_nVersion = nVersion;
}

int MySqlStorageUpdate::getVersion() {
    return m_nVersion;
}

// ----------------------------------------------------------------------

MySqlStorageAllUpdates::MySqlStorageAllUpdates() {
    TAG = "MySqlStorageAllUpdates";
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0001());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0002());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0003());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0004());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0005());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0006());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0007());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0008());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0009());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0010());
    m_vDatabaseUpdates.push_back(new MySqlStorageUpdate_0011());
    
};

bool MySqlStorageAllUpdates::upgrade(MySqlStorageConnection *pConn) {
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
    return true;
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

// ----------------------------------------------------------------------
// MySqlStorageUpdate_0010

MySqlStorageUpdate_0010::MySqlStorageUpdate_0010() 
: MySqlStorageUpdate(10) {
    TAG = "MySqlStorageUpdate_0010";
}

bool MySqlStorageUpdate_0010::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_files ADD dt DATETIME DEFAULT '2000-01-01 00:00:00'; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_files (add dt)");
        return false;
    }

    bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_files ADD f_size int(11) DEFAULT 0; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_files (add f_size)");
        return false;
    }

    bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_files ADD f_compress_size int(11) DEFAULT 0; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_files (add f_compress_size)");
        return false;
    }

    bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_files ADD f_mode VARCHAR(10) DEFAULT '----------'; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_files (add f_mode)");
        return false;
    }

    bResult = pConn->executeQuery(
        " ALTER TABLE webdiff_files ADD is_dir tinyint(1) DEFAULT 0; "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not modify table webdiff_files (add is_dir)");
        return false;
    }

    return true;
}


// ----------------------------------------------------------------------
// MySqlStorageUpdate_0011

MySqlStorageUpdate_0011::MySqlStorageUpdate_0011() 
: MySqlStorageUpdate(11) {
    TAG = "MySqlStorageUpdate_0011";
}

bool MySqlStorageUpdate_0011::apply(MySqlStorageConnection *pConn) {
    bool bResult = pConn->executeQuery(
        " CREATE INDEX webdiff_files_define_file_id ON webdiff_files(define_file_id); "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create index webdiff_files_define_file_id");
        return false;
    }

    bResult = pConn->executeQuery(
        " CREATE INDEX webdiff_files_define_file_id_version_id ON webdiff_files(define_file_id, version_id); "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create index webdiff_files_define_file_id_version_id");
        return false;
    }

    bResult = pConn->executeQuery(
        " CREATE INDEX webdiff_files_define_file_id_ver_group ON webdiff_files(define_file_id, version_id, file_group_id); "
    );
    if (!bResult) {
        WsjcppLog::err(TAG, "Could not create index webdiff_files_define_file_id_ver_group");
        return false;
    }
    return true;
}