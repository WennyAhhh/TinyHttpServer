#include "httpcontext.h"

using namespace std;

const char *HttpContext::srcDir;

void HttpContext::init()
{
    writeBuff_.retrieve_all();
    readBuff_.retrieve_all();
}

ssize_t HttpContext::read(Buffer *buff)
{
    ssize_t len = -1;
    readBuff_.append(buff->retrieve_all_string());
    process();
    return readBuff_.read_able_bytes();
}

Buffer &HttpContext::write()
{
    return writeBuff_;
}

bool HttpContext::process()
{
    request_.Init();
    if (readBuff_.read_able_bytes() <= 0)
    {
        return false;
    }
    else if (request_.parse(readBuff_))
    {
        LOG_DEBUG("%s", request_.path().c_str());
        response_.Init(srcDir, request_.path(), request_.IsKeepAlive(), 200);
    }
    else
    {
        response_.Init(srcDir, request_.path(), false, 400);
    }

    response_.MakeResponse(writeBuff_);
    if (response_.FileLen() > 0 && response_.File())
    {
        writeBuff_.append(response_.File(), response_.FileLen());
    }
    return true;
}
