#pragma once
#include <iostream>
#include "httpRequest.hpp"
#include "../tcp/buffer.h"
#include "util.hpp"
#include <ctype.h>
class HttpContext
{
public:
    const int max_line = 8192;
    enum HttpRcvStatus
    {

        RCV_ERROR,
        RCV_LINE,
        RCV_HEAD,
        RCV_CONTENT,
        RCV_OVER

    };

private:
    int resp_statu_code_;
    HttpRcvStatus RcvStatu_; // 接收状态
    HttpRequest httpReq_;    // 解析的http请求
private:
    bool
    RcvReqLine(Buffer *buffer)
    {
        if (RcvStatu_ != RCV_LINE)
            return false;
        std::string line = buffer->GetLine();
        if (line.size() == 0)
        {
            if (buffer->ReadAbleBytes() > max_line)
            {
                RcvStatu_ = RCV_ERROR;
                resp_statu_code_ = 414;
                return false;
            }
            // 不足一行直接退出
            return true;
        }
        else if (line.size() > max_line)
        {
            RcvStatu_ = RCV_ERROR;
            resp_statu_code_ = 414;
            return false;
        }

        bool ret = ParseReqLine(line);
        if (ret == true)
        {
            RcvStatu_ = RCV_HEAD;
        }
        return ret;
    }
    bool ParseReqLine(const std::string &line)
    {
        std::regex re("(GET|HEAD|PUT|POST|DELETE) ([^?]*)(?:\\?(.*))? (HTTP/1\\.[01])(?:\r\n|\n|\r)?", std::regex::icase);
        std::smatch match;
        bool ret = std::regex_match(line, match, re);
        if (ret == false)
        {
            RcvStatu_ = RCV_ERROR;
            resp_statu_code_ = 400; // bad Request;
            return false;
        }
        // 获取请求方法
        httpReq_.method_ = match[1];
        for (auto &c : httpReq_.method_)
        {
            c = std::toupper(c);
        }

        // 请求路径获取并解码,解码不需要+转空格
        httpReq_.request_path_ = util::urlDeCode(match[2], false);
        // 版本获取
        httpReq_.version_ = match[4];
        for (auto &c : httpReq_.version_)
        {
            c = std::toupper(c);
        }

        // 查询字符串获取
        std::vector<std::string> search_arr;
        util::SplitString(match[3], "&", &search_arr);
        for (auto &one_search : search_arr)
        {
            size_t pos = one_search.find("=");
            if (pos == std::string::npos)
            {
                continue;
            }
            std::string key = util::urlDeCode(one_search.substr(0, pos), true);
            std::string value = util::urlDeCode(one_search.substr(pos + 1), true);
            httpReq_.SetParams(key, value);
        }
        return true;
    }

    bool RcvReqHead(Buffer *buffer)
    {
        if (RcvStatu_ != RCV_HEAD)
            return false;
        // 一行行取直到遇到/r/n空行

        while (1)
        {

            std::string line = buffer->GetLine();
            if (line == "\n" || line == "\r\n")
                break;
            if (line.size() == 0)
            {
                if (buffer->ReadAbleBytes() > max_line)
                {
                    RcvStatu_ = RCV_ERROR;
                    resp_statu_code_ = 414;
                    return false;
                }
                // 不足一行直接退出
                return true;
            }
            else if (line.size() > max_line)
            {
                RcvStatu_ = RCV_ERROR;
                resp_statu_code_ = 414;
                return false;
            }
            else
            {
                if (line[line.size() - 1] == '\n')
                    line.pop_back();
                if (line[line.size() - 1] == '\r')
                    line.pop_back();
                bool ret = ParseReqHead(line);
                if (ret == false)
                    return false;
            }
        }
        RcvStatu_ = RCV_CONTENT;
        return true;
    }

    bool ParseReqHead(const std::string &line)
    {
        // key: value
        size_t pos = line.find(": ");
        if (pos == std::string::npos)
        {
            RcvStatu_ = RCV_ERROR;
            resp_statu_code_ = 400; // bad Request;
            return false;
        }
        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 2);

        httpReq_.SetHeader(key, val);
        return true;
    }

    bool RcvReqContent(Buffer *buffer)
    {
        if (RcvStatu_ != RCV_CONTENT)
            return false;
        // 这时候已经拿到请求头了
        // 接受正文长度
        size_t len = httpReq_.ContentSize();
        size_t needSize = len - httpReq_.content_.size();
        if (len == 0)
        { // 空报头
            RcvStatu_ = RCV_OVER;
            return true;
        }
        else
        {
            if (buffer->ReadAbleBytes() >= needSize)
            {
                // 一次性拿完 直接 return
                httpReq_.content_ += buffer->ReadAsString(needSize);
                RcvStatu_ = RCV_OVER;
                return true;
            }
            else
            { // 正文不足
                httpReq_.content_ += buffer->ReadAsString(buffer->ReadAbleBytes());
                return true; // 只有出现未知错误时返回false
            }
        }
        // 接受了多少正文
        // 接受剩下正文放到body中 考虑缓冲区中是否有剩余的全部正文
    }

public:
    HttpContext()
        : RcvStatu_(RCV_LINE), resp_statu_code_(200)
    {
    }
    int GetRespStatuCode()
    {

        return resp_statu_code_;
    }

    HttpRcvStatus GetRcvStatue()
    {
        return RcvStatu_;
    }

    HttpRequest GetHttp()
    {
        return httpReq_;
    }

    bool Rcv_Context_HttpReq(Buffer *buffer) // 接受并解析httpreq;
    {
        bool flag;

        switch (RcvStatu_)
        {
        case RCV_LINE:
            flag = RcvReqLine(buffer);
        case RCV_HEAD:
            flag = RcvReqHead(buffer);
        case RCV_CONTENT:
            flag = RcvReqContent(buffer);
        }
        return flag;
    }

    void Reset()
    {
        resp_statu_code_ = 200;;
        RcvStatu_ = RCV_LINE; // 接收状态
         httpReq_.ReSet();
    }
};