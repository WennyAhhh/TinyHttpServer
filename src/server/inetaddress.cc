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
    inet_pton(AF_INET, ip.data(), &addr_.sin_addr);
}