#include <errno.h>

#include "tcpconnection.h"
#include "event/eventloop.h"
#include "event/channel.h"
#include "base/buffer.h"
#include "socketserver.h"

void default_connection_cb(const TcpConnectionPtr &conn)
{
    LOG_INFO("%s -> %s is %s", conn->get_local_address().to_ip_port().data(),
             conn->get_peer_address().to_ip_port().data(),
             conn->is_connected() ? "UP" : "DOWN");
}

void default_message_cb(const TcpConnection &, std::shared_ptr<Buffer> buf)
{
    buf->retrieve_all();
}

void default_message_cb(const TcpConnection &, size_t len)
{
    LOG_INFO("high water mark: %d", len);
}

TcpConnection::TcpConnection(EventLoop *loop,
                             const std::string &name_arg,
                             int sockfd,
                             const InetAddress &local_address,
                             const InetAddress &peer_address)
    : loop_(loop),
      name_(name_arg),
      status_(Status::CONNECTING),
      reading_(true),
      socket_(std::make_unique<SocketServer>(sockfd)),
      channel_(std::make_unique<Channel>(loop, sockfd)),
      local_addr_(local_address),
      peer_addr_(peer_address),
      high_water_mark_(64 * 1024 * 1024) // 一次最多传输64M
{
    channel_->set_read_cb(std::bind(&TcpConnection::handle_read_, this));
    channel_->set_write_cb(std::bind(&TcpConnection::handle_write_, this));
    channel_->set_close_cb(std::bind(&TcpConnection::handle_close_, this));
    channel_->set_error_cb(std::bind(&TcpConnection::handle_error_, this));

    LOG_INFO("TcpConnection[ %s ] at %p fd =  %d", name_.data(), this, sockfd);
    // 心跳机制， 三次握手之后设置
    socket_->set_keep_alive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG("TcpConnection::~TcpConnection[ %s ] at %p fd = %d status = %s",
              name_,
              channel_->fd(),
              status_str[status_]);
    assert(status_ == Status::DISCONNECTED);
}

void TcpConnection::send(const void *data, int len)
{
    assert(len >= 0);
    send(std::string(static_cast<const char *>(data), len));
}

void TcpConnection::send(const std::string_view &message)
{
    if (status_ == Status::CONNECTED)
    {
        if (loop_->is_in_loop_thread())
        {
            send_in_loop_(message.data(), message.size());
        }
        else
        {
            loop_->run_in_loop(std::bind(send_in_loop_, this, message.data(), message.size()));
        }
    }
}

void TcpConnection::send(Buffer &buff)
{
    if (status_ == Status::CONNECTED)
    {
        if (loop_->is_in_loop_thread())
        {
            send_in_loop_(buff.peek(), buff.read_able_bytes());
            buff.retrieve_all();
        }
        else
        {
            loop_->run_in_loop(std::bind(send_in_loop_, this, buff.peek(), buff.read_able_bytes()));
        }
    }
}

void TcpConnection::shutdown()
{
    if (status_ == Status::CONNECTED)
    {
        set_status_(Status::DISCONNECTING);
        loop_->run_in_loop(std::bind(&TcpConnection::shutdown_in_loop_, this));
    }
}

void TcpConnection::send_in_loop_(const void *data, size_t len)
{
    loop_->assert_in_loop();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool fault_error = false;
    if (status_ == Status::DISCONNECTED)
    {
        LOG_WARN("diconnected, give up writing");
        return;
    }

    if (!channel_->is_writing() && output_buffer_->read_able_bytes() == 0)
    {
        nwrote = ::send(socket_->fd(), data, len, 0);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
            if (remaining == 0 && write_complete_cb_)
            {
                loop_->queue_in_loop(std::bind(write_complete_cb_, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            // 非阻塞操作， 针对不同系统
            if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                LOG_WARN("TcpConnection::send_in_loop");
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    fault_error = true;
                }
            }
        }
    }

    assert(remaining <= len);
    // 如果一次性接收不完
    // 如果超过高水位
    if (!fault_error && remaining > 0)
    {
        size_t old_len = output_buffer_->read_able_bytes();
        if (old_len >= high_water_mark_)
        {
            LOG_WARN("TcpConnection output_buffer_ is already full, Please check whether the network is clear")
        }
        if (old_len + remaining >= high_water_mark_ && old_len < high_water_mark_ && high_water_mark_cb_)
        {
            loop_->queue_in_loop(std::bind(high_water_mark_cb_, shared_from_this(), old_len + remaining));
        }
        output_buffer_->append(static_cast<const char *>(data) + nwrote, remaining);
        if (!channel_->is_writing())
        {
            channel_->enable_writing();
        }
    }
}