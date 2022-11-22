#include "sql_server.h"
#include "sql_session.h"
#include <memory>

using boost::asio::ip::tcp;
using namespace sql;

Server::Server(boost::asio::io_context &io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
    do_accept();
    commandHandler = std::make_shared<SqlHandler>("test_database");
}

void Server::do_accept()
{
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                std::make_shared<Session>(std::move(socket), commandHandler)->start();
            }

            do_accept();
        });
}