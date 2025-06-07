#include "../muduo/include/http/httpServer.h"
#include "oj_control.hpp"
#include <iostream>
#include <gflags/gflags.h>
using namespace ns_control;
DEFINE_bool(mode, false, "debug or relese");
DEFINE_string(file_name, "log.txt", "logname");
DEFINE_int32(level, 0, "level 0-4");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(base_service, "/service", "服务目录"); //
DEFINE_string(care_service, "/service/CompileRun", "rpc服务");
int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_log(FLAGS_mode, FLAGS_file_name, FLAGS_level);

    // 用户请求的服务路由功能
    std::shared_ptr<Control> ctrl = std::make_shared<Control>(FLAGS_care_service, FLAGS_etcd_host);
    // ip地址、端口号
    HttpServer svr("0.0.0.0", 8888, true); // 创建服务器对象
    svr.SetThreadNum(3);                   // 线程数量
    svr.SetBaseDir("./wwwroot");           // 服务器根目录

    // 获取所有的题目列表
    svr.Get("/all_questions", [ctrl](const HttpRequest &req, HttpResPonse *resp)
            {
            //返回一张包含有所有题目的html网页
            std::string html;
          
            ctrl->Allquestions(&html);
            //用户看到的是什么呢？？网页数据 + 拼上了题目相关的数据
            resp->SetContent(html, "text/html; charset=utf-8"); });

    // 用户要根据题目编号，获取题目的内容
    // /question/100 -> 正则匹配
    // R"()", 原始字符串raw string,保持字符串内容的原貌，不用做相关的转义
    svr.Get(R"(/question/(\d+))", [ctrl](const HttpRequest &req, HttpResPonse *resp)
            {
            std::string number = req.match_[1];
            std::string html;
            ctrl->Onequestion(number, &html);
            resp->SetContent(html, "text/html; charset=utf-8"); });

    // 用户提交代码，使用我们的判题功能(1. 每道题的测试用例 2. compile_and_run)
    svr.Post(R"(/judge/(\d+))", [ctrl](const HttpRequest &req, HttpResPonse *resp)
             {
                 std::string number = req.match_[1];
                 std::string result_json;

                 uint64_t taskid = ctrl->Judge(number, req.content_, &result_json);
                 Json::Value root;
                 root["id"] = taskid;
                 root["ok"] = true;
                 Json::FastWriter writer;
                 std::string sendto = writer.write(root);
                 resp->SetContent(sendto, "application/json;charset=utf-8");
             });
    svr.Get(R"(/GetResult/(\d+))", [ctrl](const HttpRequest &req, HttpResPonse *resp)
            {
        std::string taskid_ = req.match_[1];
        std::string outjosn;
          ctrl->GetResult(taskid_, &outjosn);
        if (outjosn.size() > 0)
            resp->SetContent(outjosn, "application/json;charset=utf-8"); });

    svr.Listen();

    return 0;
}
