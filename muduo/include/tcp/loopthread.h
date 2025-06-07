#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
class EventLoop;
class LoopThread
{

private:

    //同步获取loop 和实例化,必须实例化后才能获取
    std::mutex mtx_;
    std::condition_variable con_;
    std::thread loopthread_;

    EventLoop *loop_;

private:
    // 实例化 eventloop 运行loop;
    void ThreadEntry();

public:
    LoopThread();

    EventLoop *GetLoop() ;
};