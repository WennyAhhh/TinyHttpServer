#include "tcpserver.h"
#include "tcpconnection.h"
#include "acceptor.h"
#include "event/eventloop.h"
#include "inetaddress.h"
#include "eventloopthreadpool.h"

TcpServer::TcpServer(EventLoop *loop,
                     const InetAddress &listen_address,
                     const std::string &name_arg,
                     bool option)
    : loop_(loop),
      ip_port_(listen_address.to_ip_port()),
      name_(name_arg),
      acceptor_(std::make_unique<Acceptor>(loop, listen_address, option)),
      thread_pool_(std::make_unique<EventLoopThreadPool>(loop, name_)),
      connection_cb_(default_connection_cb),
      message_cb_(default_message_cb),
      next_id(1)
{
    acceptor_->set_new_conn_callback(
        std::bind(&TcpServer::new_connection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    loop_->assert_in_loop();
    for (auto &pItem : connection_)
    {
        TcpConnectionPtr conn(pItem.second);
        pItem.second.reset();
        conn->get_loop()->run_in_loop(std::bind(&TcpConnection::destroy_connect, conn));
    }
}

void TcpServer::start()
{
    thread_pool_->start(thread_init_cb_);

    assert(!acceptor_->listening());
    loop_->run_in_loop(std::bind(&Acceptor::listen, acceptor_.get()));
}

void TcpServer::set_thread_num(int num_threads)
{

    assert(num_threads >= 0);
    thread_pool_->set_thread_num(num_threads);
}

void TcpServer::new_connection(int sockfd, const InetAddress &peer_address)
{
}

void TcpServer::remove_connection(const TcpConnectionPtr &conn)
{
}

void TcpServer::remove_connection_in_loop(const TcpConnectionPtr &conn)
{
}
