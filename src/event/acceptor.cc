#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "acceptor.h"
#include "event/eventloop.h"

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listen_addr, bool reuseport)
    : loop_(loop),
      accept_socket_(std::make_unique<SocketServer>(create_listenfd(listen_addr.get_family()))),
      accept_channel_(std::make_unique<Channel>(loop, accept_socket_->fd())),
      idle_fd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    assert(idle_fd_ >= 0);
    accept_socket_->set_reuse_addr(true);
    accept_socket_->set_reuse_port(reuseport);
    accept_socket_->bind(listen_addr);
    accept_channel_->set_read_cb(std::bind(&Acceptor::handle_read_, this));
}

Acceptor::~Acceptor()
{
    ::close(idle_fd_);
}

void Acceptor::listen()
{
    loop_->assert_in_loop();
    listening_ = true;
    accept_socket_->listen();
    accept_channel_->enable_reading();
}

void Acceptor::handle_read_()
{
    loop_->assert_in_loop();
    InetAddress peeraddr;

    int connfd = accept_socket_->accept(peeraddr);
    if (connfd >= 0)
    {
        if (new_conn_callback_)
        {
            new_conn_callback_(connfd, peeraddr);
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        LOG_WARN("in Accrptor handle");
        // 描述符不够用
        if (errno == EMFILE)
        {
            ::close(idle_fd_);
            idle_fd_ = accept_socket_->accept();
            ::close(idle_fd_);
            idle_fd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}