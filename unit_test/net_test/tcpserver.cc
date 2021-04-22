#include "server/tcpserver.h"
#include "event/eventloop.h"
#include "server/inetaddress.h"

#include <stdio.h>

void onConnection(const TcpConnectionPtr &conn)
{
    if (conn->is_connected())
    {
        printf("onConnection(): new connection [%s] from %s\n",
               conn->get_name().c_str(),
               conn->get_peer_address().to_ip_port().c_str());
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

    // printf("onMessage(): [%s]\n", buf->retrieve_all_string().c_str());
    conn->send(buf->retrieve_all_string());
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    InetAddress listenAddr(9981);
    EventLoop loop;

    TcpServer server(&loop, listenAddr, "tcpserver", true);
    server.set_thread_num(5);
    server.set_connection_cb(onConnection);
    server.set_message_cb(onMessage);
    server.set_thread_init_cb_(initCallback);
    server.start();

    loop.loop();
}