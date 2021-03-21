#pragma once
#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <cassert>

class SqlConnPool{
public:
    // 单例模式
    static SqlConnPool* Instance();
    
    MYSQL* GetConn();
    void FreeConn(MYSQL* conn);
    int GetFreeConnCount();

    void init(const char* host, int port,
              const char* user, const char* pwd,
              const char* dbName, int connSize);
    void ClosePool();

private:
    SqlConnPool();
    ~SqlConnPool();
    
    int MAX_CONN_;
    int free_count_;
    int use_count_;

    std::queue<MYSQL* > conn_que_;
    std::mutex mtx_;
    
    sem_t sem_id_;
};

#endif