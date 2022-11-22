#ifndef SQL_SERVER_H
#define SQL_SERVER_H

#include "../sql_handler/sql_handler.h"

#include <boost/asio.hpp>

namespace sql
{
    class Server
    {
    public:
        Server(boost::asio::io_context &io_context, short port);
    private:
        void do_accept();
        boost::asio::ip::tcp::acceptor acceptor_;
        std::shared_ptr<sql::SqlHandler> commandHandler = nullptr;
    };
} // namespace  bulk

#endif // BULK_SERVER_H