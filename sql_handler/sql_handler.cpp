#include "sql_handler.h"

#include <iostream>
#include <vector>

using namespace sql;

static std::vector<std::string> split(std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos)
    {
        if (stop != 0)
            r.push_back(str.substr(0, stop));
        str.erase(0, stop + 1);
        stop = str.find_first_of(d, 0);
    }

    r.push_back(str);

    return r;
}

int readCallback(void *context, int columns, char **data, char **names)
{
    std::string *answer = reinterpret_cast<std::string *>(context);
    if (!columns)
    {
        return 0;
    }

    if (!names)
    {
        return 0;
    }

    if (!data)
    {
        return 0;
    }

    for (int i = 0; i < columns; ++i)
    {
        if (i != 0)
        {
            std::cout << ',';
            *answer += ',';
        }
        if (data[i] != NULL)
        {
            std::cout << data[i];
            *answer += data[i];
        }
    }
    std::cout << std::endl;
    *answer += '\n';

    return 0;
};

SqlHandler::SqlHandler(const std::string &name) : dbName(name)
{
    if (sqlite3_open((dbName + ".sqlite").c_str(), &handler))
    {
        std::cout << "sqlite3_open failed! " << sqlite3_errmsg(handler) << std::endl;
    }

    createTable("A");
    createTable("B");

    std::cout << "Successfully opened!" << std::endl;
}

std::string SqlHandler::handle(std::string cmd)
{
    std::string result = "ERROR: no such command\n";
    std::vector<std::string> data = split(cmd, ' ');
    if (!data.empty())
    {
        if (data[0] == "INSERT" && data.size() == 4)
        {
            result = insert(data[1], std::stoi(data[2]), data[3]);
        }
        else if (data[0] == "TRUNCATE" && data.size() == 2)
        {
            result = truncate(data[1]);
        }
        else if (data[0] == "INTERSECTION" && data.size() == 1)
        {
            result = intersection();
        }
        if (data[0] == "SYMMETRIC_DIFFERENCE" && data.size() == 1)
        {
            result = symmetric_difference();
        }
    }
    return result;
}

std::string SqlHandler::insert(const std::string &table,
                               int id,
                               const std::string &name)
{
    std::string result = "OK\n";
    std::string cmd = "INSERT INTO " + table + " VALUES(" + std::to_string(id) + ", '" + name + "');";
    char *errMsg = nullptr;
    if (sqlite3_exec(handler, cmd.c_str(), /*callback*/ nullptr, /*cb 1st argument*/ nullptr, &errMsg))
    {
        result = "ERR " + std::string(errMsg);
        std::cout << "sqlite3_exec with createQuery failed! " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    std::cout << "Successfully inserted!" << std::endl;
    return result;
}

std::string SqlHandler::truncate(const std::string &table)
{
    std::string result = "OK\n";
    std::string query = "DELETE FROM " + table + ";";
    char *errMsg = nullptr;
    if (sqlite3_exec(handler, query.c_str(), /*callback*/ nullptr, /*cb 1st argument*/ nullptr, &errMsg))
    {
        result = "ERR " + std::string(errMsg);
        std::cout << "sqlite3_exec with truncate query failed! " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    std::cout << "Successfully truncated!" << std::endl;
    return result;
}

std::string SqlHandler::intersection()
{
    std::string result;
    const std::string query = "select A.id, A.name, B.name from A inner join B on B.id = A.id;";
    // const std::string query = "SELECT * FROM A INTERSECT SELECT * FROM B;";
    char *errMsg = nullptr;
    if (sqlite3_exec(handler, query.c_str(), /*callback*/ readCallback, /*cb 1st argument*/ &result, &errMsg))
    {
        result = "ERR " + std::string(errMsg);
        std::cout << "sqlite3_exec with intersection failed! " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    std::cout << "Successfully intersection!" << std::endl;
    return result;
}

std::string SqlHandler::symmetric_difference()
{
    std::string result;
    const std::string query(std::string("select A.id, A.name, B.name from A left outer join B on A.id = B.id ") +
                            std::string("except ") +
                            std::string("select A.id, A.name, B.name from A inner join B on B.id = A.id ") +
                            std::string("union ") +
                            std::string("select B.id, A.name, B.name from B left outer join A on A.id = B.id ") +
                            std::string("except ") +
                            std::string("select A.id, A.name, B.name from A inner join B on B.id = A.id;"));
    char *errMsg = nullptr;
    if (sqlite3_exec(handler, query.c_str(), /*callback*/ readCallback, /*cb 1st argument*/ &result, &errMsg))
    {
        result = "ERR " + std::string(errMsg);
        std::cout << "sqlite3_exec with symmetric_difference failed! " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    std::cout << "Successfully symmetric_difference!" << std::endl;
    return result;
}

void SqlHandler::createTable(const std::string &name)
{
    const std::string query = "CREATE TABLE " + name + "(id PRIMARY KEY, name);";
    char *errMsg = nullptr;
    if (sqlite3_exec(handler, query.c_str(), /*callback*/ nullptr, /*cb 1st argument*/ nullptr, &errMsg))
    {
        std::cout << "sqlite3_exec with create query failed! " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    std::cout << "Successfully created!" << std::endl;
}