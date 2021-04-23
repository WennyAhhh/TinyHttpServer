#include "httpserver.h"

void HttpServer::message_cb(const TcpConnectionPtr &conn, std::shared_ptr<Buffer> &buff)
{
    extend_time(conn);
    printf("onMessage(): received %zd bytes from connection [%s]\n",
           buff->read_able_bytes(),
           conn->get_name().c_str());
    process(conn, buff);
    // printf("onMessage(): [%s]\n", buf->retrieve_all_string().c_str());
    // conn->send(buff->retrieve_all_string());
}

void HttpServer::init_cb(EventLoop *loop)
{
    LOG_INFO("init");
}

void HttpServer::process(const TcpConnectionPtr &conn, std::shared_ptr<Buffer> &readBuff)
{
    HttpResponse response;
    HttpContext *context = std::any_cast<HttpContext>(conn->get_context_ptr());
    context->Init();
    if (readBuff->read_able_bytes() <= 0)
    {
        return;
    }
    else if (context->parse(readBuff))
    {
        LOG_DEBUG("%s", context->path().c_str());
        response.Init(src_dir_, context->path(), context->IsKeepAlive(), 200);
    }
    else
    {
        response.Init(src_dir_, context->path(), false, 400);
    }
    Buffer writeBuff;
    response.MakeResponse(writeBuff);
    /* 响应头 */
    // iov_[0].iov_base = const_cast<char *>(writeBuff_.peek());
    // iov_[0].iov_len = writeBuff_.ReadableBytes();
    // iovCnt_ = 1;
    conn->send(writeBuff);
    /* 文件 */

    if (response.FileLen() > 0 && response.File())
    {
        Buffer fileBuffer;
        fileBuffer.append(response.File(), response.FileLen());
        conn->send(fileBuffer);
        // iov_[1].iov_base = response_.File();
        // iov_[1].iov_len = response_.FileLen();
        // iovCnt_ = 2;
    }
    // LOG_DEBUG("filesize:%d, %d  to %d", response.FileLen(), iovCnt_, ToWriteBytes());
}

void HttpServer::connection_cb(const TcpConnectionPtr &conn)
{
    extend_time(conn);
    conn->set_context(HttpContext());
    // StampPtr stamp(conn, std::bind(&TcpConnection::shutdown, conn));
    // connection_list_.back().push_back(stamp);

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

void HttpServer::extend_time(const TcpConnectionPtr &conn)
{
    EntryPtr entry(new Entry(conn));
    connection_list_.back().push_back(entry);
}
