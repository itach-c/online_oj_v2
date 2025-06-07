#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <unistd.h>
#include <sys/timerfd.h>
#include "../comm/log.hpp"

class Channel;
class EventLoop;

// 定义任务的回调类型
using TaskFunc = std::function<void()>;
// 定义任务释放回调类型
using ReleseFunc = std::function<bool()>;

// TimeTask类：用于封装定时任务
class TimeTask
{
public:
    TimeTask(uint64_t id, uint32_t timeout, TaskFunc cb);
    ~TimeTask();

    void cancelTask();
    void setRelesaeCb(ReleseFunc cb);
    uint32_t getTimeout();

private:
    uint64_t id_;
    uint32_t timeout_;
    TaskFunc taskcb_;
    ReleseFunc releasecb_;
    bool canceled_;
};

// TimeWheel类：时间轮，管理定时任务
class TimeWheel
{
    using taskPtr = std::shared_ptr<TimeTask>;
    using weakTask = std::weak_ptr<TimeTask>;

public:
    TimeWheel(EventLoop *loop);
    ~TimeWheel();

    // 刷新任务
    void timeAdd(uint64_t id, uint32_t timeout, const TaskFunc cb);

    // 刷新任务
    void reflashTimer(uint64_t id);

    // 执行定时任务
    void runTimerTask();

    // 取消任务
    void timeCancel(uint64_t id);

    bool Hastimer(uint64_t id) { return !(timers_.find(id) == timers_.end()); }

private:
    // 移除任务
    bool timerRemove(uint64_t id);
    int CreatTimeFd();

    int ReadTimeFd();
    void Ontime();

    // 添加定时任务

    void timeAddInloop(uint64_t id, uint32_t timeout, const TaskFunc cb);

    // 刷新任务
    void reflashTimerInLoop(uint64_t id);
    // 取消任务
    void timeCancelInLoop(uint64_t id);

private:
    uint32_t tick_;                                 // 每次走一步，最小单位由调用者实现
    int capacity_;                                  // 最大计时时间
    std::vector<std::vector<taskPtr>> wheel_;       // 时间轮
    std::unordered_map<uint64_t, weakTask> timers_; // 存储任务id对应的weak_ptr
    int timefd_;
    EventLoop *loop_;
    std::unique_ptr<Channel> timefd_channel_;
};
