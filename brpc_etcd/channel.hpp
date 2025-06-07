#pragma once
#include <brpc/channel.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "spdlog.hpp"
class ServiceChannel
{
public:
    using ChannelPtr = std::shared_ptr<brpc::Channel>;
    using Ptr = std::shared_ptr<ServiceChannel>;

public:
    ServiceChannel(const std::string &name)
        : service_name_(name), index_(0)
    {
    }
    bool append(const std::string &host)
    {
        brpc::ChannelOptions options;
        options.connect_timeout_ms = -1;

        options.protocol = "baidu_std"; // rpc协议，默认是百度std
        options.max_retry = 3;          // 当与服务器丢失连接时重试的最大次数
        auto channel = std::make_shared<brpc::Channel>();
        int ret = channel->Init(host.c_str(), &options);
        if (ret == -1)
        {
            SLOG_ERROR("{} - {}节点主机初始化信道失败", service_name_, host);
            return false;
        }
        std::unique_lock<std::mutex> lock(mtx_);
        hosts_.insert(std::make_pair(host, channel));
        channels_.push_back(channel);
        return true;
    }
    bool remove(const std::string &host)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        auto it = hosts_.find(host);
        if (it == hosts_.end())
        {
            SLOG_WARN("{}服务的{}主机在hosts中并不存在", service_name_, host);
            return false;
        }
        for (auto vit = channels_.begin(); vit != channels_.end(); vit++)
        {
            if (*vit == it->second)
            {
                channels_.erase(vit);
                hosts_.erase(it);
                return true;
            }
        }
        return false;
    }
    // 通过rr轮转获取一个信道发起rpc调用
    ChannelPtr choose()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (channels_.size() == 0)
        {
            SLOG_ERROR("当前没有能提供 {} 服务的节点", service_name_);
            return nullptr;
        }
        index_++;
        index_ = index_ % channels_.size();
        return channels_[index_];
    }

private:
    int32_t index_;
    std::string service_name_;         // 服务名称
    std::vector<ChannelPtr> channels_; // 一个服务对应多个信道
    std::unordered_map<std::string, ChannelPtr> hosts_;
    std::mutex mtx_;
};

class ServiceChannelManager
{

public:
    ServiceChannel::ChannelPtr choose(const std::string &name)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        auto sit = Services_.find(name);
        if (sit == Services_.end())
        {
            SLOG_ERROR("当前没有能提供 {} 服务的节点", name);
            return nullptr;
        }
        return sit->second->choose();
    }

    void Declear(const std::string &service_dir)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        CareServices_.insert(service_dir);
    }
    void OnServiceOffline(const std::string &service_name, const std::string &host)
    {
        std::string name = GetServiceName(service_name);
        auto careit = CareServices_.find(name);
        if (careit == CareServices_.end())
        {
            SLOG_INFO("{} 服务下线了但是我们并不关心", name);
            return;
        }

        ServiceChannel::Ptr Service;
        {
            std::unique_lock<std::mutex> lock(mtx_);
            auto sit = Services_.find(name); // ✅ 修正这里
            if (sit == Services_.end())
            {
                SLOG_WARN("删除的服务管理对象 {} 并不存在", name);
                return;
            }
            Service = sit->second;
        }

        Service->remove(host);
    }

    void OnServiceOnline(const std::string &service_name, const std::string &host)
    {
        std::string name = GetServiceName(service_name); // 提取出前缀，比如 /service/CompileRun
        auto careit = CareServices_.find(name);
        if (careit == CareServices_.end())
        {
            SLOG_INFO("{} 服务上线了但是我们并不关心", name);
            return;
        }
        std::cout << "上线服务名称： " << name << " host: " << host << std::endl;

        ServiceChannel::Ptr Service;
        {
            std::unique_lock<std::mutex> lock(mtx_);
            auto sit = Services_.find(name); // ✅ 修正这里
            if (sit == Services_.end())
            {
                SLOG_INFO("没管理这个服务直接注册 {}", name);
                Service = std::make_shared<ServiceChannel>(name);
                Services_.insert(std::make_pair(name, Service));
            }
            else
            {
                Service = sit->second;
            }
        }
        if (!Service)
        {
            SLOG_ERROR("新增 {} 管理节点失败", name);
            return;
        }
        Service->append(host);
    }

    void debug()
    {
        auto it = Services_.begin();
        while (it != Services_.end())
        {
            std::cout << "services name " << it->first << " ";
            it++;
        }
    }

private:
    std::string GetServiceName(const std::string &name)
    {
        int pos = name.rfind('/');
        return name.substr(0, pos);
    }

private:
    std::mutex mtx_;
    std::unordered_map<std::string, ServiceChannel::Ptr> Services_;
    std::unordered_set<std::string> CareServices_;
};