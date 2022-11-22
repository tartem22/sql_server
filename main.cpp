#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include "sql_handler/sql_handler.h"
#include "server/sql_server.h"

void testOnlySql()
{
    auto sqlHandler = sql::SqlHandler("Test");
    std::string cmd = "TRUNCATE A";
    sqlHandler.handle(cmd);
    cmd = "TRUNCATE B";
    sqlHandler.handle(cmd);
    cmd = "INSERT A 0 lean";
    sqlHandler.handle(cmd);
    cmd = "INSERT A 0 understand";
    sqlHandler.handle(cmd);
    cmd = "INSERT A 1 sweater";
    sqlHandler.handle(cmd);
    cmd = "INSERT A 2 frank";
    sqlHandler.handle(cmd);
    cmd = "INSERT B 2 franki";
    sqlHandler.handle(cmd);
    cmd = "INSERT B 6 flour";
    sqlHandler.handle(cmd);
    cmd = "INSERT A 7 wonder";
    sqlHandler.handle(cmd);
    cmd = "INSERT B 7 wonderful";
    sqlHandler.handle(cmd);
    cmd = "INSERT B 8 selection";
    sqlHandler.handle(cmd);
    cmd = "INTERSECTION";
    sqlHandler.handle(cmd);
    cmd = "SYMMETRIC_DIFFERENCE";
    sqlHandler.handle(cmd);
}

void testServer(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            argv[1] = "9000";
        }

        boost::asio::io_context io_context;

        sql::Server server(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
    }
}

int main(int argc, char *argv[])
{
    // testOnlySql();
    testServer(argc, argv);
    return 0;
}