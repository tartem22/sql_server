#include "sql_session.h"

#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace sql;

static std::vector<std::string> split(const char *c_str, std::size_t size)
{
    std::string str;
    std::vector<std::string> r;
    r.reserve(1000);

    for (std::size_t i = 0; i < size; i++)
    {
        if (*c_str == '\n')
        {
            if (str.length() != 0)
            {
                // str += *c_str;
                r.push_back(str);
                str.clear();
            }
        }
        else
            str += *c_str;
        c_str++;
    }
    r.shrink_to_fit();
    return r;
}

Session::Session(boost::asio::ip::tcp::socket socket,
                 std::shared_ptr<SqlHandler> &handler) : socket_(std::move(socket)),
                                                         handler(handler)
{
}

Session::~Session()
{
    execution.store(false);
    while(processingRun.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Session::start()
{
    execution.store(true);
    std::thread(&Session::processing, this).detach();
    do_read();
}

void Session::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            [this, self](boost::system::error_code ec, std::size_t length)
                            {
                                if (!ec)
                                {
                                    do_process(length);
                                }
                            });
}

void Session::do_process(std::size_t length)
{
    auto self(shared_from_this());
    std::vector<std::string> cmds = split(data_, length);
    for (const auto &cmd : cmds)
    {
        cmdQueue.push(cmd);
    }
    queueCV.notify_one();
    do_read();
}

void Session::processing()
{
    auto self(shared_from_this());
    processingRun.store(true);
    while (execution.load())
    {
        std::unique_lock lock(queueMutex);
        queueCV.wait(lock);

        while (!cmdQueue.empty())
        {
            std::string cmd = cmdQueue.front();
            cmdQueue.pop();
            std::string ansver = handler->handle(cmd);
            boost::asio::async_write(socket_, boost::asio::buffer(ansver, ansver.size()),
                                     [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                     });
        }
    }
    processingRun.store(false);
}