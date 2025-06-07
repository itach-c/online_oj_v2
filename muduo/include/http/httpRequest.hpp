#pragma once
#include <iostream>
#include <unordered_map>
#include <regex>
class HttpRequest
{

public:
    // 请求头

    std::string method_;
    // url
    std::string request_path_; // 请求路径
    std::string version_;      // http版本
    bool keep_alive;
    std::smatch match_;
    std::string content_;                                        // 正文
    std::unordered_map<std::string, std::string> headers_;       // 请求头
    std::unordered_map<std::string, std::string> search_params_; // 查询参数键值对

public:
    void ReSet()
    {
        HttpRequest tmp;

        std::swap(*this, tmp);
    }
    void SetHeader(const std::string &key, const std::string &val)
    {
        headers_[key] = val;
    }

    bool HasHeader(const std::string &key) // 判断是否有指定头
    {
        auto it = headers_.find(key);
        return it != headers_.end();
    }
    std::string GetHeader(const std::string &key)

    {

        if (false == HasHeader(key))
            return "";

        return headers_[key];
    }
    void SetParams(const std::string &key, const std::string &val)
    {
        search_params_[key] = val;
    }
    bool HasParam(const std::string &key) // 判断是否有指定头
    {
        auto it = search_params_.find(key);
        return it != search_params_.end();
    }
    std::string GetParams(const std::string &key)
    {

        if (false == HasParam(key))
            return "";

        return search_params_[key];
    }
    size_t ContentSize()
    {
        if (HasHeader("Content-Length") == false)
        {
            return 0;
        }

        std::string size = GetHeader("Content-Length");

        return std::stol(size);
    }
    bool isShortConnection()
    {
        if (HasHeader("Connection") && GetHeader("Connection") == "keep-alive")
            return false;

        return true;
    }
};