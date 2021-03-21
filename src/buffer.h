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

class Buffer
{
public:
    // 内联常量
    static constexpr size_t cheap_prepend = 8;
    static constexpr size_t initial_size_default = 5;
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
    ssize_t readFd(int fd, int *saveErrno);
    // 返回可读区域
    size_t ReadAbleBytes() const
    {
        return writer_index_ - reader_index_;
    }
    // 返回可写区域
    size_t WriteAbleBytes() const
    {
        return buffer_.size() - writer_index_;
    }
    // 返回预留区域
    size_t PrependAbleBytes() const
    {
        return reader_index_;
    }
    const char *peek() const
    {
        return begin() + reader_index_;
    }
    void Retrieve(size_t len)
    {
        assert(len <= ReadAbleBytes());
        if (len < ReadAbleBytes())
        {
            reader_index_ += len;
        }
        else
        {
            RetrieveAll();
        }
    }
    void RetrieveAll()
    {
        reader_index_ = cheap_prepend;
        writer_index_ = cheap_prepend;
    }
    void RetrieveUnitil(const char *end)
    {
        // 通用的end设定
        assert(peek() <= end);
        assert(end <= BeginWrite());
        // 如果相等的话， 不会读取
        Retrieve(end - peek());
    }
    std::string RetrieveAllAsString()
    {
        return RetrieveAsString(ReadAbleBytes());
    }
    std::string RetrieveAsString(size_t len)
    {
        assert(len <= ReadAbleBytes());
        std::string res(peek(), len);
        Retrieve(len);
        return res;
    }
    const char *BeginWrite() const
    {
        return begin() + writer_index_;
    }

    char *BeginWrite() noexcept
    {
        return begin() + writer_index_;
    }

    void append(const char *data, size_t len)
    {
        EnsureWritAbleBytes(len);
        std::copy(data, data + len, BeginWrite());
        HasWritten(len);
    }
    void append(const std::string_view data)
    {
        append(data.data(), data.size());
    }
    void append(const void *data, size_t len)
    {
        append(static_cast<const char *>(data), len);
    }
    void prepend(const std::string_view data, size_t len)
    {
        // 可以后挪
        assert(len <= PrependAbleBytes());
        reader_index_ -= len;
        std::copy(data.begin(), data.begin() + len, begin() + reader_index_);
    }
    void shrink()
    {
        //buffer_.shrink_to_fit();
        std::vector<char>(buffer_).swap(buffer_);
    }
    // 可能抛出异常
    void EnsureWritAbleBytes(size_t len)
    {
        if (WriteAbleBytes() < len)
        {
            MakeSpace(len);
        }
    }
    void HasWritten(size_t len) noexcept
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
    const int GetSize() const
    {
        return buffer_.capacity();
    }

private:
    char *begin()
    {
        return &*buffer_.begin();
    }
    const char *begin() const
    {
        return &*buffer_.begin();
    }
    void MakeSpace(size_t len);
    std::vector<char> buffer_;
    size_t reader_index_;
    size_t writer_index_;

    static const char kCRLF[];
};
#endif