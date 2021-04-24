#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <sys/types.h>
#include <sys/uio.h>   // readv/writev
#include <arpa/inet.h> // sockaddr_in
#include <stdlib.h>    // atoi()
#include <errno.h>

#include "base/log.h"
#include "sql/sqlconnRAII.h"
#include "base/buffer.h"
#include "httprequest.h"
#include "httpresponse.h"

class HttpContext
{
public:
    HttpContext() = default;

    ~HttpContext() = default;

    void init();

    ssize_t read(Buffer *buff);

    Buffer &write();

    bool process();

    static const char *srcDir;

private:
    Buffer readBuff_;  // 读缓冲区
    Buffer writeBuff_; // 写缓冲区

    HttpRequest request_;
    HttpResponse response_;
};

#endif //HTTP_CONN_H