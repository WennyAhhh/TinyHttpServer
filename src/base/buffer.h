#pragma once
#ifndef BUFFER_H
#define BUFFER_H

#include <cassert>
#include <cstring>
#include <string_view>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/uio.h>
#include <unistd.h>

class Buffer
{
public:
    // 内联常量
    static constexpr size_t cheap_prepend = 8;
    static constexpr size_t initial_size_default = 1024;
    explicit Buffer(size_t initial_size = initial_size_default)
        : buffer_(cheap_prepend + initial_size), // buffer长度
          reader_index_(cheap_prepend),
          writer_index_(cheap_prepend)
    {
    }
    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;

    // 类的访问控制是针对类， 而不是针对对象
    void swap(Buffer &rhs) noexcept
    {
        using std::swap;
        buffer_.swap(rhs.buffer_);
        swap(reader_index_, rhs.reader_index_);
        swap(writer_index_, rhs.writer_index_);
    }
    ssize_t read_fd(int fd, int *save_rrrno);
    ssize_t write_fd(int fd, int *save_errno);

    size_t read_able_bytes() const { return writer_index_ - reader_index_; }
    size_t write_able_bytes() const { return buffer_.size() - writer_index_; }
    size_t prepend_able_bytes() const { return reader_index_; }

    const char *peek() const { return begin_() + reader_index_; }
    void retrieve(size_t len)
    {
        assert(len <= read_able_bytes());
        if (len < read_able_bytes())
        {
            reader_index_ += len;
        }
        else
        {
            retrieve_all();
        }
    }
    void retrieve_all()
    {
        reader_index_ = cheap_prepend;
        writer_index_ = cheap_prepend;
    }
    void retrieve_unitil(const char *end)
    {
        // 通用的end设定
        assert(peek() <= end);
        assert(end <= begin_write());
        // 如果相等的话， 不会读取
        retrieve(end - peek());
    }
    std::string retrieve_all_string() { return retrieve_string(read_able_bytes()); }
    std::string retrieve_string(size_t len)
    {
        assert(len <= read_able_bytes());
        std::string res(peek(), len);
        retrieve(len);
        return res;
    }

    const char *begin_write_const() const { return begin_() + writer_index_; }

    char *begin_write() noexcept { return begin_() + writer_index_; }

    void append(const std::string_view data) { append(data.data(), data.size()); }
    void append(const void *data, size_t len) { append(static_cast<const char *>(data), len); }
    void append(const char *data, size_t len)
    {
        ensure(len);
        std::copy(data, data + len, begin_write());
        has_written(len);
    }

    void prepend(const std::string_view data, size_t len)
    {
        // 可以后挪
        assert(len <= prepend_able_bytes());
        reader_index_ -= len;
        std::copy(data.begin(), data.begin() + len, begin_() + reader_index_);
    }
    void shrink()
    {
        //buffer_.shrink_to_fit();
        std::vector<char>(buffer_).swap(buffer_);
    }
    // 可能抛出异常
    void ensure(size_t len)
    {
        if (write_able_bytes() < len)
        {
            makespace(len);
        }
    }
    void has_written(size_t len) noexcept
    {
        writer_index_ += len;
    }

    void print() const
    {
        for (auto &e : buffer_)
        {
            std::cout << e;
        }
        std::cout << std::endl;
    }
    const int size() const
    {
        return buffer_.capacity();
    }

private:
    char *begin_()
    {
        return &*buffer_.begin();
    }
    const char *begin_() const
    {
        return &*buffer_.begin();
    }
    void makespace(size_t len);
    std::vector<char> buffer_;
    size_t reader_index_;
    size_t writer_index_;

    static const char kCRLF[];
};
#endif