#pragma once

// 系统头文件
#include <iostream>
#include <vector>
#include <fstream>
#include <mutex>
#include <atomic>
#include <memory>
#include <arpa/inet.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <cstring>

// 第三方库头文件
#include "jsoncpp/json/json.h"

// 用户自定义的头文件
// #include "oj_model2.hpp"
#include "oj_model.hpp" // 如果需要可以解除注释
#include "oj_view.hpp"
#include "../comm/myutile/CommnUtil.hpp"
#include "../compile_server/compile_run.hpp"
#include "../brpc_etcd/channel.hpp"
#include "../brpc_etcd/etcd.hpp"
#include "../comm/message.pb.h"

namespace ns_control
{
    using namespace ns_model;

    using namespace ns_view;



    const int redis_data_destroy_time = 20; // redis数据ttl

    struct rpcdata
    {

        brpc::Controller *ctrl_;
        itach_oj::CompileRunResponse *resp_;
        uint64_t taskid_;
    };
    class Control
    {

    public:
        Control(const std::string &service_name, const std::string &etcdhost)
            : taskid_(1), pservice_manager_(std::make_shared<ServiceChannelManager>()), service_name_(service_name)
        {
            pservice_manager_->Declear(service_name);
            auto Online = std::bind(&ServiceChannelManager::OnServiceOnline, pservice_manager_.get(), std::placeholders::_1, std::placeholders::_2);
            auto Offline = std::bind(&ServiceChannelManager::OnServiceOffline, pservice_manager_.get(), std::placeholders::_1, std::placeholders::_2);
            pdis_ = std::make_shared<Discover>(etcdhost, service_name, Online, Offline);
            pdis_->async_wait();
        }
        bool Allquestions(std::string *html)
        {
            std::vector<question> all;
            if (model_.GetAllQuestions(&all))
            {
                // 将所有题目构建成网页返回
                std::sort(all.begin(), all.end(), [](question a, question b)
                          { return std::stoi(a.qid) < std::stoi(b.qid); });

                view_.all_expand_html(html, all);
                return true;
            }
            else
            {
                *html = "获取题目列表失败";
                return false;
            }
        }
        bool Onequestion(std::string &num, std::string *html,std::string language = "c_cpp")
        {

            question q;
            if (model_.GetOneQuestion(num, &q) == true)
            {

                // 构建html返回
                view_.oneExpandHtml(html, q,language);
                return true;
            }
            else
            {
                *html = "获取指定题目失败,题目编号:" + num;
                return false;
            }
        }
        
        bool Language_change(std::string &num, std::string *langCode,std::string language = "c_cpp"){
            question q;
            if (model_.GetOneQuestion(num, &q) == true)
            {
                *langCode = q.header[language];
            }
            else
            {
                *langCode= "Language_change faild:" + num;
                return false;
            }


        }


        void GetResult(const std::string &taskid, std::string *out_json)
        {
            *out_json = model_.GetResult(taskid);
        }
        uint64_t Judge(const std::string &num, const std::string &in_json, std::string *out_json)
        {
            std::cout<<"=========== in_json printf start =============="<<std::endl;
            std::cout<<""<<in_json<<std::endl;
            std::cout<<"=========== in_json printf end =============="<<std::endl;

            // 1接受前端传来的代码
            question q;
            model_.GetOneQuestion(num, &q);
            Json::Value root;

            // 2 反序列化得到用户代码和用户输入
            Json::Reader reader;
            reader.parse(in_json, root);
            std::string code = root["code"].asString();
            
            std::string input = root["input"].asString();
            std::string lang = root["language"].asString();
            
            auto it = q.header.find(lang);
            if (it != q.header.end() && it->second == code)
                return -1;
            std::cout<<"language is "<<lang<<std::endl;
            // 选择信道发起rpc调用;
            itach_oj::CompileRunRequest request;
            request.set_code(code + q.tail[lang]);
            request.set_input(input);
            request.set_cpu_limit(q.cpu_limit);
            request.set_mem_limit(q.mem_limit);
            
            /*
            enum Language {
                CPP = 0;   // C++
                JAVA = 1;  // Java
                GO = 2;    // Go
                }
                
                Language language = 5;  // 指定编程语言
                */
            if (!lang.empty()) {
                if (lang == "c_cpp") {
                    request.set_language(itach_oj::CompileRunRequest::CPP);
                } else if (lang == "java") {
                    request.set_language(itach_oj::CompileRunRequest::JAVA);
                } else if (lang == "go") {
                    request.set_language(itach_oj::CompileRunRequest::GO);
                }
            }
            else{
                MYLOG_ERROR("没有选择有效语言");
            }

               
            // 选择一个信道
            auto channel = pservice_manager_->choose(service_name_);
            if (channel == nullptr)
            {
                MYLOG_ERROR("所有服务器已经下线请立刻检查");
                return 0;
            }
            itach_oj::CompileRunService_Stub stub(channel.get());
            auto *ctrl = new brpc::Controller();
            auto *response = new itach_oj::CompileRunResponse();
            rpcdata data;
            uint64_t ret = 0;
            ctrl->set_timeout_ms(3000);
            std::lock_guard<std::mutex> lock(mtx_);
            {
                data.ctrl_ = ctrl;
                data.resp_ = response;
                data.taskid_ = taskid_;
                ret = taskid_;
                tasks_.insert(data.taskid_);
                taskid_++;
            }
            auto closure = google::protobuf::NewCallback(this, &Control::callback, data);
            stub.Start(ctrl, &request, response, closure);
            std::cout << "rpc异步调用结束" << std::endl;

            return ret;
        }

    private:
        void callback(rpcdata data)
        { // unique_ptr会帮助我们在return时自动删掉response/cntl，防止忘记。gcc 3.4下的unique_ptr是模拟版本。
            std::unique_ptr<itach_oj::CompileRunResponse> resp_gurd(data.resp_);
            std::unique_ptr<brpc::Controller> ctlr_guard(data.ctrl_);
            if (ctlr_guard->Failed())
            {
                // RPC失败了. response里的值是未定义的，勿用。

                MYLOG_ERROR("RPC调用失败error reason: %s", ctlr_guard->ErrorText().c_str());
            }
            else
            {
                // RPC成功了，response里有我们想要的数据。开始RPC的后续处理。
                std::string reason = resp_gurd->reason();
                std::string out = resp_gurd->proto_stdout();
                std::string err = resp_gurd->proto_stderr();
                int stauts = resp_gurd->status();

                int passnum = resp_gurd->pass_num();
                int totalnum = resp_gurd->total_num();
                std::string LastErrorInput = resp_gurd->last_error_input();
                Json::Value root;
                if (stauts >= 0)
                {
                    root["stdout"] = out;
                    root["stderror"] = err;
                    root["passnum"] = passnum;
                    root["totalnum"] = totalnum;
                    root["LastErrorInput"] = LastErrorInput;
                }
                root["status"] = stauts;
                root["reason"] = reason;
                Json::FastWriter writer;
                std::string sendtoclient = writer.write(root);
                std::cout << sendtoclient << std::endl;
                // 这里应该存在redis里比较合理//unordered_map 没有超时删除功能;
                bool ret = model_.SetResult(std::to_string(data.taskid_), sendtoclient, redis_data_destroy_time);
                if (ret == false)
                {
                    MYLOG_ERROR("存入redis失败taskid_:%ld", data.taskid_);
                }
                MYLOG_INFO("RPC异步调用成功 taskid: %ld", data.taskid_);
            }
            // NewCallback产生的Closure会在Run结束后删除自己，不用我们做。
        }
        Model model_;
        View view_;

        uint64_t taskid_;
        std::shared_ptr<ServiceChannelManager> pservice_manager_;
        std::unordered_set<uint64_t> tasks_;
        std::shared_ptr<Discover> pdis_;
        std::string service_name_;
        std::mutex mtx_;
    };
}