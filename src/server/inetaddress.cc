#include "inetaddress.h"

InetAddress::InetAddress(uint16_t port, bool loopback)
{
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    // 选择本地环回地址还是任意地址, 如果有多网卡建议任意地址
    addr_.sin_addr.s_addr = htonl(loopback ? INADDR_LOOPBACK : INADDR_ANY);
    addr_.sin_port = htons(port);
}

InetAddress::InetAddress(std::string_view ip, uint16_t port)
{
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.data(), &addr_.sin_addr) <= 0)
    {
        LOG_ERROR("InetAddress inet_pton");
    }
}

InetAddress::InetAddress(int sockfd)
{
    memset(&addr_, 0, sizeof addr_);
    socklen_t len = static_cast<socklen_t>(sizeof addr_);
    // 获取本地协议地址
    if (::getsockname(sockfd, reinterpret_cast<sockaddr *>(&addr_), &len) < 0)
    {
        LOG_ERROR("InetAddress::InetAddress can not get scokfd");
        memset(&addr_, 0, sizeof addr_);
    }
}

const std::string InetAddress::to_ip_port() const
{
    assert(addr_.sin_family == AF_INET);
    char buf[64];
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, static_cast<socklen_t>(sizeof buf));
    std::string ip = std::string(buf);
    unsigned short int ports = ::ntohs(addr_.sin_port);
    return ip + ":" + std::to_string(ports);
}

const sockaddr *InetAddress::get_addr() const
{
    return reinterpret_cast<const sockaddr *>(&addr_);
}