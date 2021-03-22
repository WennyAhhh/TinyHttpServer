#include <thread>
#include <memory>
#include "../src/log.h"

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void AssertInLoop();
    bool IsInLoopThread();
    void AbortNotLoopThread();
    std::shared_ptr<EventLoop> GetEventLoopCurrentThread();

private:
    std::thread::id thread_id;
    bool looping_;
    std::atomic<int> res;
};