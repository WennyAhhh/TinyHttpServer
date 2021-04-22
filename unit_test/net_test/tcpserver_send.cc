#include "server/tcpserver.h"
#include "event/eventloop.h"
#include "server/inetaddress.h"

#include <stdio.h>

std::string message1;
std::string message2;

void onConnection(const TcpConnectionPtr &conn)
{
    if (conn->is_connected())
    {
        printf("onConnection(): new connection [%s] from %s\n",
               conn->get_name().c_str(),
               conn->get_peer_address().to_ip_port().c_str());
        conn->send(message1);
        conn->send(message2);
        conn->shutdown();
    }
    else
    {
        printf("onConnection(): connection [%s] is down\n",
               conn->get_name().c_str());
    }
}

void initCallback(EventLoop *)
{
    printf("xiao mi\n");
}

void onMessage(const TcpConnectionPtr &conn,
               std::shared_ptr<Buffer> &buf)
{
    printf("onMessage(): received %zd bytes from connection [%s]\n",
           buf->read_able_bytes(),
           conn->get_name().c_str());

    printf("onMessage(): [%s]\n", buf->retrieve_all_string().c_str());
    // conn->send(buf->retrieve_all_string());
}

int main(int argc, char *argv[])
{
    printf("main(): pid = %d\n", getpid());

    int len1 = 100;
    int len2 = 200;

    if (argc > 2)
    {
        len1 = atoi(argv[1]);
        len2 = atoi(argv[2]);
    }

    message1.resize(len1);
    message2.resize(len2);
    std::fill(message1.begin(), message1.end(), 'A');
    std::fill(message2.begin(), message2.end(), 'B');

    InetAddress listenAddr(9981);
    EventLoop loop;

    TcpServer server(&loop, listenAddr, "tcpserver", true);
    server.set_connection_cb(onConnection);
    server.set_message_cb(onMessage);
    server.set_thread_init_cb_(initCallback);
    server.start();

    loop.loop();
}