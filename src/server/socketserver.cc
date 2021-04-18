#include "socketserver.h"

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

int create_listenfd(sa_family_t family)
{
    int listenfd_ = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (listenfd_)
    {
        LOG_ERROR("Acceptor listenfd");
    }
    return listenfd_;
}

void SocketServer::bind(const InetAddress &addr)
{
    const sockaddr *address = addr.get_addr();
    int ret = ::bind(sockfd_, address, sizeof(*address));
    if (ret < 0)
    {
        LOG_WARN("sockets::bind");
    }
}

void SocketServer::listen()
{
    int ret = ::listen(sockfd_, MAXBACKLOG);
    if (ret < 0)
    {
        LOG_WARN("socket listen");
    }
}

int SocketServer::accept(InetAddress &peeraddr)
{
    sockaddr_in client;
    memset(&client, 0, sizeof client);
    socklen_t client_addrlength = sizeof client;
    int connfd = ::accept(sockfd_, reinterpret_cast<sockaddr *>(&client), &client_addrlength);
    if (connfd < 0)
    {
        int savedErrno = errno;
        LOG_WARN("Socket::accept");
        switch (savedErrno)
        {
        case EAGAIN:
        case ECONNABORTED:
        case EINTR:
        case EPROTO:
        case EPERM:
        case EMFILE:
            errno = savedErrno;
            break;
        case EBADF:
        case EFAULT:
        case EINVAL:
        case ENFILE:
        case ENOBUFS:
        case ENOMEM:
        case ENOTSOCK:
        case EOPNOTSUPP:
            LOG_ERROR("unexpected error of ::accept %d", savedErrno);
            break;
        default:
            LOG_ERROR("unknown error of ::accept %d", savedErrno);
            break;
        }
    }
    else
    {
        peeraddr.set_addr(client);
    }
    return connfd;
}

int SocketServer::accept()
{
    int connfd = ::accept(sockfd_, NULL, NULL);
    return connfd;
}

void SocketServer::set_reuse_addr(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
}

void SocketServer::set_reuse_port(bool on)
{
// linux kernel >= 3.9
// 端口重用， 多个Bind和listen可以绑定一个端口
#ifdef SO_REUSEPORT
    int opt = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
    if (ret < 0 && on)
    {
        LOG_ERROR("SO_REQUESEPORT faild");
    }
#else
    if (on)
    {
        LOG_ERROR("SO_REQUESEPORT is not supported");
    }
#endif
}