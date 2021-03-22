#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool
{
public:
    explicit ThreadPool(size_t);
    template <class F, class... Args> //可变参数模版
    //值得注意的是这里F&&表示universal reference而不是右值引用
    //如果存在推断类型如template或auto那么&&即表示universal reference，具体是左值引用还是右值引用由初始化决定
    auto enqueue(F &&f, Args &&...args)       //f是函数名，args是参数
        -> std::future<decltype(f(args...))>; //尾置返回类型，返回 函数f返回值类型的future
    ~ThreadPool();

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks; //std::function通用的函数封装，要求一个返回值类型为void的无参函数

    // synchronization
    std::mutex queue_mutex;            //锁，负责保护任务队列和stop
    std::condition_variable condition; //条件变量
    bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads) //构造时设定线程数量
    : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back( //push_back的优化版本
            [this]            //lambda表达式捕获this指针
            {
                for (;;) //比while(1)更优
                {
                    std::function<void()> task;
                    { //{}内相当于新的作用域
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        //在等待任务队列中出现任务的过程中解锁queue_mutex
                        //由notify_one或notify_all唤醒
                        //线程池初始化后将有threads个线程在此处等待，每个线程执行完分配到的任务将执行循环，再取任务执行或等待任务加入队列
                        /* 我们需要知道这么做的目的是，std::thread本身仅能绑定一个函数，而我们需要仅用threads个线程去帮我们执行m个任务，
                             * 而不是每执行一个任务创建一个线程，如果这样我们将创建m个线程，而创建线程是需要开销的，这引起了不必要的浪费，线程池就是为此而生的
                             * 通过这种方式，每个std::thread仍然是只绑定了一个函数，但是这一个函数会执行我们想要的多个任务
                             */
                        this->condition.wait(lock,
                                             [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty()) //stop=true，仍需执行任务队列中剩余任务
                            return;
                        task = std::move(this->tasks.front()); //std::move避免拷贝
                        this->tasks.pop();
                    }

                    task(); //执行任务
                }
            });
}
// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&...args)
    -> std::future<decltype(f(args...))>
{
    using return_type = decltype(f(args...));

    //基本类型是std::shared_ptr，指向类型是std::packaged_task，类型是返回值类型为return_type的无参函数
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        /* 现在该说说为什么std::packaged_task的类型是一个返回值为return_type的无参数函数了
             * 返回值是return_type这没有问题，至于参数消失的原因是因为：std::bind
             * 在这里它创建了一个无参数（参数均被指定）版本的函数f
             */
        //std::forward配合universal reference使用，完美转发，实际效果是如果是右值引用那么还是右值引用，如果是左值引用那么还是左值引用
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future(); //任务函数实际执行后的返回值
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); }); //往tasks队列压入一个无参无返回值的函数，函数体内调用task（不要忘记task是shared_ptr类型）
    }
    //任务压入队列，唤醒等待的线程
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all(); //唤醒所有等待的进程
    for (std::thread &worker : workers)
        worker.join(); //等待所有线程结束
}

#endif