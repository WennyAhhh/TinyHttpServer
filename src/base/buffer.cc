#include "buffer.h"

void Buffer::makespace(size_t len)
{
    // std::cout << buffer_.capacity() << std::endl ;
    if (write_able_bytes() + prepend_able_bytes() < len + cheap_prepend)
    {
        buffer_.resize(writer_index_ + len);
    }
    else
    {
        assert(cheap_prepend < reader_index_);
        size_t readable = read_able_bytes();
        // std::copy(begin() + reader_index_,
        //           begin() + writer_index_,
        //           begin() + cheap_prepend);
        memmove(begin_() + cheap_prepend,
                begin_() + reader_index_,
                writer_index_ - cheap_prepend);
        reader_index_ = cheap_prepend;
        writer_index_ = cheap_prepend + readable;
        assert(readable == read_able_bytes());
    }
}

ssize_t Buffer::readFd(int fd, int *saveErrno)
{
    char extrabuf[65535];
    struct iovec vec[2];
    size_t write_size = write_able_bytes();
    vec[0].iov_base = begin_() + writer_index_;
    vec[0].iov_len = write_size;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    const ssize_t len = readv(fd, vec, 2);
    if (len < 0)
    {
        *saveErrno = errno;
    }
    else if (static_cast<size_t>(len) <= write_size)
    {
        writer_index_ += len;
    }
    else
    {
        writer_index_ = buffer_.size();
        append(extrabuf, len - write_size);
    }
    return len;
}
