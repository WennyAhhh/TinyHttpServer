#include "sqlconnpool.h"

SqlConnPool::SqlConnPool(){
    use_count_ = 0;
    free_count_ = 0;
}

void SqlConnPool::init(const char* host, int port,
    const char* user, const char* pwd, const char* db_name,
    int conn_size){

    assert(conn_size > 0);
    for(int i = 0; i < conn_size; i++){
        MYSQL *sql = nullptr;
        sql = mysql_init(sql);
        if(!sql) {
            // LOG
            assert(sql);
        }
        sql = mysql_real_connect(sql, host, 
                                 user, pwd, db_name,
                                 port, nullptr, 0);
        if(!sql){
            // LOG
        }
        conn_que_.push(sql);
    }
    MAX_CONN_ = conn_size;
    // 设置信号量。 是否可以去掉
    sem_init(&sem_id_, 0, MAX_CONN_);
}

 MYSQL* SqlConnPool::GetConn(){
    MYSQL* sql = nullptr;
    if(conn_que_.empty()){
        // LOG
        return nullptr;
    }
    sem_wait(&sem_id_);
    {
        std::lock_guard<std::mutex> lk(mtx_);
        sql = conn_que_.front();
        conn_que_.pop();
    }
    return sql;
}

void SqlConnPool::FreeConn(MYSQL* sql){
    assert(sql);
    std::lock_guard<std::mutex> locker(mtx_);
    conn_que_.push(sql);
    sem_post(&sem_id_);
}

void SqlConnPool::ClosePool() {
    std::lock_guard<std::mutex> lk(mtx_);
    while(!conn_que_.empty()){
        auto sql = conn_que_.front();
        conn_que_.pop();
        mysql_close(sql);
    }
    mysql_library_end();
}

int SqlConnPool::GetFreeConnCount(){
    std::lock_guard<std::mutex> lk(mtx_);
    return conn_que_.size();
}

SqlConnPool::~SqlConnPool(){
    ClosePool();
}