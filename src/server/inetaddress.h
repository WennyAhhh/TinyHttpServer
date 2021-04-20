#pragma once
#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <netinet/in.h>
#include <string_view>
#include <string.h>
#include <arpa/inet.h>
#include "base/log.h"

int setnonblocking(int fd);
int create_listenfd(sa_family_t family);

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, bool loopback = false);
    InetAddress(std::string_view ip, uint16_t port = 0);
    InetAddress(sockaddr_in &addr) : addr_(addr) {}

    const sockaddr *get_addr() const;
    const sa_family_t get_family() const { return addr_.sin_family; }
    const std::string to_ip_port() const;

    void set_addr(const sockaddr_in &addr) { addr_ = addr; }

private:
    sockaddr_in addr_;
};

#endif