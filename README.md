# TinyHttpServer

## 进度
    LOG多线程日志库， 基本功能实现， 未实现自动切换LOG文件
    Buffer缓冲
## 后续
    线程池
    epoll封装
    set时间事件
    事件队列
    http(打算移植过来)
## 一些小问题
    如果创建之后立刻退出， cond发出通知， 但是LOG线程还没有走到绑定的函数当中去， 造成assert错误