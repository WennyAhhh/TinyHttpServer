#include "httpserver.h"
#include <any>

void HttpServer::message_cb(const TcpConnectionPtr &conn, Buffer *buff)
{
    WeakEntryPtr weakEntry(std::any_cast<WeakEntryPtr>(conn->get_entry()));
    HttpContext *context = conn->get_context_ptr();
    EntryPtr entry(weakEntry.lock());
    if (entry)
    {
        connection_list_.back().push_back(entry);
        context->read(buff);
        conn->send(context->write());
        // dumpConnectionBuckets();
    }
    // extend_time(conn);
    printf("onMessage(): received %zd bytes from connection [%s]\n",
           buff->read_able_bytes(),
           conn->get_name().c_str());

    // printf("onMessage(): [%s]\n", buf->retrieve_all_string().c_str());
    // conn->send(buff->retrieve_all_string());
}

void HttpServer::init_cb(EventLoop *loop)
{
    LOG_INFO("init");
}

void HttpServer::connection_cb(const TcpConnectionPtr &conn)
{
    // extend_time(conn);
    // conn->set_context(HttpContext());
    // StampPtr stamp(conn, std::bind(&TcpConnection::shutdown, conn));
    // connection_list_.back().push_back(stamp);
    if (conn->is_connected())
    {
        EntryPtr entry(new Entry(conn));
        connection_list_.back().push_back(entry);
        WeakEntryPtr weakEntry(entry);
        conn->set_entry(weakEntry);
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

void HttpServer::extend_time(const TcpConnectionPtr &conn)
{
    EntryPtr entry(new Entry(conn));
    connection_list_.back().push_back(entry);
}
