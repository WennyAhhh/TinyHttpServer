#pragma once
#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
#include <string_view>
#include <any>
#include <map>

#include "base/buffer.h"
#include "inetaddress.h"

class Channel;
class EventLoop;
class SocketServer;
class TcpConnection;
class TimerNode;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void()> TimerCallBack;
typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;
typedef std::function<void(const TcpConnectionPtr &, size_t)> HighWaterMarkCallback;
typedef std::function<void(const TcpConnectionPtr &, Buffer *)> MessageCallback;

// class Status
// {
// public:
//     static const int DISCONNECTED = 0;
//     static const int CONNECTING = 1;
//     static const int CONNECTED = 2;
//     static const int DISCONNECTING = 3;
// };

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop,
                  const std::string &name,
                  int sockfd,
                  const InetAddress &local_address,
                  const InetAddress &peer_address);
    ~TcpConnection();

    EventLoop *get_loop() const { return loop_; }
    const std::string &get_name() const { return name_; }
    const InetAddress &get_local_address() const { return local_addr_; }
    const InetAddress &get_peer_address() const { return peer_addr_; }

    bool is_connected() const { return status_ == Status::CONNECTED; }

    void send(const void *message, int len);
    void send(const std::string_view &message);
    void send(Buffer &message);
    void shutdown();
    void force_close();
    void force_close_with_delay(double second);

    void start_read();
    void stop_read();
    bool is_reading() const { return reading_; }

    void set_connection_cb(const ConnectionCallback &cb) { connection_cb_ = cb; }
    void set_message_cb(const MessageCallback &cb) { message_cb_ = cb; }
    void set_write_complete_cb(const WriteCompleteCallback &cb) { write_complete_cb_ = cb; }
    void set_close_cb(const CloseCallback &cb) { close_cb_ = cb; }
    void set_high_water_mark_cb(const HighWaterMarkCallback &cb, size_t high_water_mark)
    {
        high_water_mark_cb_ = cb;
        high_water_mark_ = high_water_mark;
    }

    std::shared_ptr<Buffer> input_buffer() { return input_buffer_; }
    std::shared_ptr<Buffer> output_buffer() { return output_buffer_; }

    void build_connect();
    void destroy_connect();

private:
    enum class Status
    {
        DISCONNECTED = 0,
        CONNECTING,
        CONNECTED,
        DISCONNECTING
    };
    std::map<Status, std::string> status_str = {
        {Status::DISCONNECTED, "DISCONNECTED"},
        {Status::CONNECTING, "CONNECTING"},
        {Status::CONNECTED, "CONNECTED"},
        {Status::DISCONNECTING, "DISCONNECTING"}};

    void handle_read_();
    void handle_write_();
    void handle_close_();
    void handle_error_();

    void set_in_loop_(const std::string_view &message);
    void send_in_loop_(const void *message, size_t len);
    void shutdown_in_loop_();
    void start_read_in_loop_();
    void stop_read_in_loop_();
    void force_close_in_loop_();

    void set_status_(Status s);

    EventLoop *loop_;
    const std::string name_;
    Status status_;
    bool reading_;

    std::unique_ptr<SocketServer> socket_;
    std::unique_ptr<Channel> channel_;

    const InetAddress local_addr_;
    const InetAddress peer_addr_;
    ConnectionCallback connection_cb_;
    MessageCallback message_cb_;
    WriteCompleteCallback write_complete_cb_;
    HighWaterMarkCallback high_water_mark_cb_;
    CloseCallback close_cb_;

    size_t high_water_mark_;
    std::shared_ptr<Buffer> input_buffer_;
    std::shared_ptr<Buffer> output_buffer_;
    std::any context_;
};

#endif