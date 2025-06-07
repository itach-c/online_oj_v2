#pragma once
#include <iostream>
#include "socket.h"
#include "channel.h"
#include "memory"
#include <cassert>
class EventLoop;
class Acceptor
{

private:
    using NewConnectionCallback = std::function<void(int fd)>;
    EventLoop *loop_;
    Socket listen_socket_;
    std::unique_ptr<Channel> listen_channel_;

    NewConnectionCallback accept_callback_;

private:
    void HandleRead();

public:
    Acceptor(EventLoop* loop,const std::string &ip,uint32_t port,bool reusePort = false);

    void SetNewConnectionCallback(const NewConnectionCallback &cb);
};