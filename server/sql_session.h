#ifndef BULK_SESSION_H
#define BULK_SESSION_H

#include <memory>
#include <thread>
#include <condition_variable>
#include <queue>
#include <boost/asio.hpp>
#include "../sql_handler/sql_handler.h"

namespace sql
{
    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        Session(boost::asio::ip::tcp::socket socket,
                std::shared_ptr<SqlHandler> &handler);
        ~Session();
        void start();

    private:
        void do_read();
        void do_write();
        void do_process(std::size_t length);
        void processing();
        boost::asio::ip::tcp::socket socket_;
        std::shared_ptr<SqlHandler> handler = nullptr;
        std::queue<std::string> cmdQueue;
        std::mutex queueMutex;
        std::condition_variable queueCV;
        std::atomic<bool> execution;
        std::atomic<bool> processingRun;
        enum
        {
            max_length = 1024
        };
        char data_[max_length];
    };
} // namespace bulk

#endif // BULK_SESSION_H