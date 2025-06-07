#pragma once
#include <iostream>
#include <sys/epoll.h>
#include <unordered_map>
#include <vector>
#include "channel.h"
#include <cassert>
#include <cstring>
class Channel;
class Poller
{

    const static int MAX_EPOLLEVENTS = 1024;

public:
    Poller();
    // 添加或者修改监控
    void UpDateEvent(Channel *channel);

    // 移除监控
    void ReMoveEvent(Channel *channel);

    // 返回活跃连接
    void Poll(std::vector<Channel *> *active);

    // for test

    const char *eventToString(uint32_t events)
    {
        static const char *eventStrings[] = {
            "EPOLLIN",     // 1
            "EPOLLPRI",    // 2
            "EPOLLOUT",    // 4
            "EPOLLERR",    // 8
            "EPOLLHUP",    // 16
            "EPOLLRDHUP",  // 32
            "EPOLLMSG",    // 64
            "EPOLLET",     // 128
            "EPOLLONESHOT" // 256
        };

        static char buffer[256];
        buffer[0] = '\0';

        bool first = true;
        for (int i = 0; i < sizeof(eventStrings) / sizeof(eventStrings[0]); ++i)
        {
            if (events & (1 << i))
            {
                if (!first)
                {
                    strcat(buffer, "|");
                }
                strcat(buffer, eventStrings[i]);
                first = false;
            }
        }
        return buffer;
    }
    void printList()
    {
    
        for (auto &c : channels_)
        {
            std::cout << "fd : " << c.first << " ";
            std::cout << "events " << eventToString(c.second->Events()) << std::endl;
        }
    }

private:
    void Update(Channel *channel, int op);

    bool HasChannel(Channel *channel);

private:
    int epollfd_;
    struct epoll_event evs_[MAX_EPOLLEVENTS];
    std::unordered_map<int, Channel *> channels_;
};