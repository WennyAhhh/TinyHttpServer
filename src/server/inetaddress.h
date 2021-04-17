#pragma once
#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <netinet/in.h>
#include <string_view>
#include <string.h>
#include <arpa/inet.h>

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, bool loopback = false);
    InetAddress(std::string_view ip, uint16_t port = 0);
    InetAddress(sockaddr_in &addr) : addr_(addr) {}

private:
    sockaddr_in addr_;
};

#endif