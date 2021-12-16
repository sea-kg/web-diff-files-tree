#ifndef MYSQL_STORAGE_UPDATES_H
#define MYSQL_STORAGE_UPDATES_H

#include "mysql_storage_connection.h"

class MySqlStorageConnection;

class MySqlStorageUpdate {
    public:
        MySqlStorageUpdate(int nVersion);
        int getVersion();
        virtual bool apply(MySqlStorageConnection *) = 0;

    private:
        int m_nVersion;
};

class MySqlStorageAllUpdates {
    public:
        MySqlStorageAllUpdates();
        bool upgrade(MySqlStorageConnection *pConn);
    private:
        std::string TAG;
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

#endif // MYSQL_STORAGE_UPDATES_H

