#pragma once
#include <vector>
#include <iostream>
#include "loopthread.h"
#include "../comm/log.hpp"
class EventLoop;
class LoopThreadPool
{

private:
    int loopthread_num_;
    EventLoop *baseloop_;                  // 运行在主线程的eventloop;
    std::vector<std::unique_ptr<LoopThread>> loop_threads_; // 从reactor 数量
    int next_idx_;                         // RR轮转下标
    std::vector<EventLoop *> loops_;

public:
    LoopThreadPool(EventLoop* baseloop);
    void SetLoopThreadNum(int n);
    void Create();
    EventLoop* NextLoop();
};