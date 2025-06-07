#pragma once
#include <iostream>
#include <functional>
#include "httpRequest.hpp"
#include "httpResponse.hpp"
#include "httpContext.hpp"
#include <unordered_map>
#include "../tcp/tcpserver.h"
class HttpServer
{

private:
    using Handler = std::function<void(const HttpRequest &req, HttpResPonse *resp)>;
    using HandlerRoute = std::vector<std::pair<std::regex, Handler>>; // 存储的是正则表达式 和 处理函数
    HandlerRoute route_get_;
    HandlerRoute route_post_;
    HandlerRoute route_put_;
    HandlerRoute route_delete_;

    std::string basedir_;
    TcpServer tcpserver_;

private:
    void OnConnection(Connection::PtrConnection);              // 设置上下文
    void OnMessage(Connection::PtrConnection, Buffer *buffer); // 解析处理
    void Route(HttpRequest &req, HttpResPonse *resp);
    bool HandleStaticSource(HttpRequest &req, HttpResPonse *resp);
    void Dispatcher(HttpRequest &req, HttpResPonse *resp, HandlerRoute &route); // 功能性请求分发

    void BuildResponseAndSend(Connection::PtrConnection, HttpRequest &req, HttpResPonse &rep);
    void HandleError(HttpResPonse *resp);

    bool isStaticRequest(HttpRequest &req, HttpResPonse *resp);

public:
    HttpServer(const std::string &ip, uint32_t port, bool Portreuse = false);

    void SetBaseDir(const std::string &path);

    void Get(const std::string &pattern, Handler handler);
    void Post(const std::string &pattern, Handler handler);
    void Put(const std::string &pattern, Handler handler);
    void Delete(const std::string &pattern, Handler handler);

    void SetThreadNum(int num);
    void Handle404(HttpResPonse *resp);
    void EnableInactiveDestroy(int timeout); // 单位s

    void Listen();
};