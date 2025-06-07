#pragma once
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <etcd/Response.hpp>
#include <etcd/Watcher.hpp>
#include "spdlog.hpp"
#include <thread>
#include <stdint.h>
class Registry
{
public:
    using ptr = std::shared_ptr<Registry>;
    Registry(const std::string &host)
        : client_(std::make_shared<etcd::Client>(host)), keep_alive_(client_->leasekeepalive(3).get()), lease_id_(keep_alive_->Lease())
    {
    }
    ~Registry()
    {
        keep_alive_->Cancel();
    }
    bool registry(const std::string &key, const std::string &val)
    {
        auto resp = client_->put(key, val, lease_id_).get();
        if (!resp.is_ok())
        {
            SLOG_ERROR("注册服务失败key:{} val:{}", key, val);
            return false;
        }
        return true;
    }

private:
    std::shared_ptr<etcd::Client> client_;
    std::shared_ptr<etcd::KeepAlive> keep_alive_;
    uint64_t lease_id_;
};

class Discover
{
public:
    using OnlineCallback = std::function<void(const std::string &name, const std::string &host)>;
    using OfflineCallback = std::function<void(const std::string &name, const std::string &host)>;
    Discover(const std::string &host, const std::string &basedir, const OnlineCallback &oncb, const OfflineCallback &offcb)
        : client_(std::make_shared<etcd::Client>(host)), onlinecb_(oncb), offlinecb_(offcb)
    {
        auto resp = client_->ls(basedir).get();

        int sz = resp.keys().size();

        for (int i = 0; i < sz; i++)
        {
            if (onlinecb_)
                onlinecb_(resp.keys()[i], resp.values()[i].as_string());
        }
        watcher_ = std::make_shared<etcd::Watcher>(*client_.get(), basedir, std::bind(&Discover::callback, this, std::placeholders::_1), true);
    }
    void async_wait()
    {

        wait_thread_ = std::thread([this]()
                                   { this->watcher_->Wait(); });
    }
    void stop()
    {
        if (watcher_)
            watcher_->Cancel();
        if (wait_thread_.joinable())
            wait_thread_.join();
    }

private:
    void callback(etcd::Response resp)
    {
        if (!resp.is_ok())
        {
        }
        for (auto &ev : resp.events())
        {
            if (ev.event_type() == etcd::Event::EventType::PUT)
            {
                SLOG_INFO("{}:{} 服务上线了", ev.kv().key(), ev.kv().as_string());
                onlinecb_(ev.kv().key(), ev.kv().as_string());
            }
            else if (ev.event_type() == etcd::Event::EventType::DELETE_)
            {
                SLOG_INFO("{}:{} 服务下线了", ev.prev_kv().key(), ev.prev_kv().as_string());
                offlinecb_(ev.prev_kv().key(), ev.prev_kv().as_string());
            }
        }
    }
    std::shared_ptr<etcd::Client> client_;
    std::shared_ptr<etcd::Watcher> watcher_;
    OnlineCallback onlinecb_;
    OfflineCallback offlinecb_;
    std::thread wait_thread_;
};