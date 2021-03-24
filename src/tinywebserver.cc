#include "tinywebserver.h"

using namespace std;

WebServer::WebServer(
    int port, int trigMode, int timeoutMS, bool optLinger,
    int sqlPort, const char *sqlUser, const char *sqlPwd, const char *dbName,
    int connPoolNum, int threadNum,
    bool openLog, int logLevel, int logQueSize) : port_(port), openLinger_(optLinger), timeoutMS_(timeoutMS), isClose_(false),
                                                  timer_(make_unique<HeapTimer>()),
                                                  threadpool_(make_unique<ThreadPool>(threadNum)),
                                                  ePollBase_(make_unique<EPollBase>())
{
        srcDir_ = getcwd(nullptr, 256);
        assert(srcDir_.data());
        srcDir_ += "/resource/";
}
