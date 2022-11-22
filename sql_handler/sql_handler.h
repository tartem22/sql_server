#ifndef SQL_HANDLER_H
#define SQL_HANDLER_H

#include "../sqlite3.h"
#include <string>

namespace sql
{
    class SqlHandler
    {
    public:
        SqlHandler(const std::string &name);
        std::string handle(std::string cmd);

    private:
        std::string insert(const std::string &table,
                           int id,
                           const std::string &name);
        std::string truncate(const std::string &table);
        std::string intersection();
        std::string symmetric_difference();
        void createTable(const std::string &name);

        std::string dbName;
        sqlite3 *handler = nullptr;
    };
}

#endif