#pragma once
#include <iostream>
#include <iostream>
#include <unordered_map>
class HttpResPonse
{

private:
    int statuCode_; // 状态码

    std::string version_;

    std::unordered_map<std::string, std::string> headers_; // 响应头

    std::string content_; // 正文

    bool redirectFlag_;
    std::string redirectUrl_;

public:
    // Getter 函数
    int GetStatusCode() const
    {
        return statuCode_;
    }

    std::string GetVersion() const
    {
        return version_;
    }

    // 获取响应头（返回整个头部 map）
    std::unordered_map<std::string, std::string> GetHeaders() const
    {
        return headers_;
    }

    // 获取某个特定的响应头
    std::string GetHeader(const std::string &key) const
    {
        auto it = headers_.find(key);
        if (it != headers_.end())
        {
            return it->second;
        }
        return ""; // 如果没有找到对应的头部，返回空字符串
    }

    std::string GetContent() const
    {
        return content_;
    }

    bool GetRedirectFlag() const
    {
        return redirectFlag_;
    }

    std::string GetRedirectUrl() const
    {
        return redirectUrl_;
    }

public:
    HttpResPonse(int statu = 200)
        : redirectFlag_(false), statuCode_(statu)
    {
    }

    void ReSet()
    {

        HttpResPonse tmp;
        std::swap(*this, tmp);
    }
    void SetContent(const std::string &content, const std::string &contentType = "text/html")
    {

        content_ = content;
        SetHeader("Content-Type", contentType);
        SetHeader("Content-Length", std::to_string(content.size()));
    }
    void SetStatueCode(int code)
    {
        statuCode_ = code;
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
    void SetRedirect(const std::string &url, int code = 302)
    {
        redirectFlag_ = true;
        redirectUrl_ = url;
        statuCode_ = code;
    }

    bool isShortConnection()
    {

        if (HasHeader("Connection") && GetHeader("Connection") == "Keep-Alive")
            return false;

        return true;
    }
    void SetVersion(const std::string& version)
    {
        version_ = version;
    }
};