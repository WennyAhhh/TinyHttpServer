#include "event/eventloop.h"
#include "server/inetaddress.h"
#include "event/acceptor.h"
#include <stdio.h>

void newConnection(int sockfd, const InetAddress &peerAddr)
{
    printf("newConnection(): accepted a new connection from %s\n",
           peerAddr.to_ip_port().c_str());
    ::write(sockfd, "How are you?\n", 13);
    ::close(sockfd);
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    InetAddress listenAddr(9981);
    EventLoop loop;

    Acceptor acceptor(&loop, listenAddr, true);
    acceptor.set_new_conn_callback(newConnection);
    acceptor.listen();

    loop.loop();
}