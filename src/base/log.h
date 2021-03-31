#pragma once
#ifndef LOG_H
#define LOG_H

#include <thread>
#include <mutex>
#include <time.h>
#include <atomic>
#include <stdarg.h>
#include <sys/stat.h>
#include <condition_variable>
#include <functional>
#include "buffer.h"

class Buffer;

class Log
{
public:
    static constexpr int SIZE = 4000;
    inline static const std::string LEVEL[] = {"debug", "info", "warn", "error"};
    static Log *instance()
    {
        static Log log;
        return &log;
    }
    ~Log()
    {
        printf("~Log\n");
        if (running_)
        {
            stop();
        }
    }
    void init(
        const std::string_view path,
        bool p_TimerStamp,
        bool p_datastamp);
    bool is_open()
    {
        return is_open_;
    }
    void stop()
    {
        printf("stop");
        running_ = false;
        cond_.notify_one();
        thread_.join();
    }
    void write(const std::string &str)
    {
        std::string mess;
        if (date_stamp_)
        {
            mess += "[" + datestamp() + "] ";
        }
        if (time_stamp_)
        {
            mess += "[" + timerstamp() + "] ";
        }
        mess += str;
        append(mess.data(), mess.size());
    }

private:
    Log();
    void thread_func();
    std::string timerstamp() noexcept
    {
        char str[9];
        time_t t = time(0);
        tm *curr_time = localtime(&t);
        strftime(str, 9, "%H:%M:%S", curr_time);
        return str;
    }
    std::string datestamp() noexcept
    {
        char str[11];
        time_t t = time(0);
        tm *curr_time = localtime(&t);
        strftime(str, 11, "%Y.%m.%d", curr_time);
        return str;
    }
    void switch_log();
    void append(const char *logline, int len);

    FILE *fp_;
    std::mutex mtx_;
    const char *path_{"./.log"};
    //std::unique_ptr<std::thread> thread_;
    std::thread thread_;
    std::condition_variable cond_;
    std::atomic<bool> running_;
    std::atomic<bool> is_open_;
    std::atomic<int> cnt;
    std::unique_ptr<Buffer> curr_buffer_;
    std::unique_ptr<Buffer> next_buffer_;
    std::vector<std::unique_ptr<Buffer>> buffer_;
    bool time_stamp_{true};
    bool date_stamp_{true};
};

#define LOG(level, fmt, ...)                                                                                                                \
    do                                                                                                                                      \
    {                                                                                                                                       \
        char _buf[1024] = {0};                                                                                                              \
        snprintf(_buf, sizeof(_buf), "[%s:%s:%d][%s]" fmt "\n", __FILE__, __FUNCTION__, __LINE__, Log::LEVEL[level].data(), ##__VA_ARGS__); \
        Log::instance()->write(std::string(_buf));                                                                                          \
    } while (0);

#define LOG_DEBUG(fmt, ...)        \
    do                             \
    {                              \
        LOG(0, fmt, ##__VA_ARGS__) \
    } while (0);

#define LOG_INFO(fmt, ...)         \
    do                             \
    {                              \
        LOG(1, fmt, ##__VA_ARGS__) \
    } while (0);

#define LOG_WARN(fmt, ...)         \
    do                             \
    {                              \
        LOG(2, fmt, ##__VA_ARGS__) \
    } while (0);
#define LOG_ERROR(fmt, ...)        \
    do                             \
    {                              \
        LOG(3, fmt, ##__VA_ARGS__) \
    } while (0);
#endif