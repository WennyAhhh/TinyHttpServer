#pragma once
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <unistd.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <cassert>

using std::string;

class HeapTimer;
class ThreadPool;
class Epoller;
class HttpConn;

class WebServer {
public:
    WebServer(
        int port, int trigMode, int timeoutMS, bool optLinger,
        int sqlPort, const char* sqlUser, const char* sqlPwd, const char* dbName, 
        int connPoolNum, int threadNum,
        bool openLog, int logLevel, int logQueSize
    );
    ~WebServer();
    void Start();
private:
    bool InitSocket();
    void InitEventMode(int trigMode);

    // 应该导入一个json文件
    int port_{1316};            // 端口
    bool openLinger_{true};     //优雅关闭?
    int timeoutMS_;             // 超时时间
    bool isClose_;              // 是否关闭
    int listenFd_{3306};        // 监听端口
    string srcDir_;             // 如果是可改变的，string是否更好？

    uint32_t listenEvent_;
    uint32_t connEvent_;

    std::unique_ptr<HeapTimer> timer_;
    std::unique_ptr<ThreadPool> threadpool_;
    std::unique_ptr<Epoller> epoller_;
    std::unordered_map<int, HttpConn> users_;
    
};


#endif
