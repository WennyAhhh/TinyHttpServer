#include "event/eventloop.h"
#include "server/inetaddress.h"
#include "httpserver.h"

#include <stdio.h>

using namespace std;

int main()
{
    cout << "start" << endl;
    Log::instance()->init("./.log", true, true);
    InetAddress listenAddr(9981);
    EventLoop loop;

    HttpServer server(&loop, listenAddr, "tcpserver", true);
    char dir[] = "/resources/";
    server.set_dir(dir);
    server.start();

    loop.loop();
    return 0;
}