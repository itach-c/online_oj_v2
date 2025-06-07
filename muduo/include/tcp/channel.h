#pragma once
#include <iostream>
#include <functional>
#include <unistd.h>
#include <sys/epoll.h>
#include "../comm/log.hpp"
class EventLoop;
class Channel
{

    using EventCallBack = std::function<void()>; // todo

public:
    // 事件管理

    Channel(EventLoop *eventLoop, int fd);

    ~Channel();

    int GetFd();
    void SetReadCallBack(const EventCallBack &cb);

    uint32_t Events();

    void SetWriteCallBack(const EventCallBack &cb);

    void SetErrorCallBack(const EventCallBack &cb);

    void SetCloseCallBack(const EventCallBack &cb);

    void SetEventsCallBack(const EventCallBack &cb);

    bool ReadAble(); // 描述符是否可读

    bool WriteAble(); // 描述符是否可写

    void EnAbleRead(); // 添加读

    void EnAbleWrite(); // 添加写

    void DisAbleRead(); // 移除读

    void DisAbleWrite(); // 移除写

    void DisAbleAll(); // 移除所有

    void ReMove(); // todo

    void HandleEvent();

    void SetRevents(uint32_t events);

    // for test
    void UpDateEvent();
    void ReMoveEvent();

private:
    uint32_t events_;  // 当前连接监控的事件
    uint32_t revents_; // 当前连接触发的事件
    EventCallBack ReadCallBack_;
    EventCallBack WriteCallBack_;
    EventCallBack ErrorCallBack_;
    EventCallBack CloseCallBack_;
    EventCallBack EventsCallBack_;
    int fd_;

    EventLoop *eventloop_;
};