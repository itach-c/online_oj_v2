#pragma once
#include <iostream>
#include <unordered_map>
#include "../comm/myutile/ojlog.hpp"
#include <vector>
#include "../comm/myutile/CommnUtil.hpp"
#include <fstream>
#include "question.hpp"
#include <cpp_redis/cpp_redis>
#include <mutex>

namespace ns_model
{
  

    class Model
    {
    public:
        const std::string sep = " ";
        const std::string question_list_path = "./questions/questions.list";
        const std::string questions_path = "./questions/";

        Model()
        {
            LoadQuestionsList(question_list_path);
            InitRedis(); // 初始化 Redis
        }

        ~Model() {}

        bool LoadQuestionsList(const std::string &list_path)
        {
            std::ifstream in(list_path);
            if (!in.is_open())
            {
                MYLOG_FATAL("loadQuestion open file error errno :%d, message : %s", errno, strerror(errno));
                return false;
            }

            std::string line;
            while (std::getline(in, line))
            {
                question q;
                std::vector<std::string> v;
                StringUtile::splitString(line, sep, &v);
                if (v.size() != 5)
                {
                    MYLOG_ERROR("加载题目列表出错,题号:%s", q.qid.c_str());
                    continue;
                }

                q.qid = v[0];
                q.title = v[1];
                q.level = v[2];
                q.cpu_limit = std::stoi(v[3]);
                q.mem_limit = std::stoi(v[4]);

                std::string question_desc_path = questions_path + q.qid + "/desc.txt";
                std::string question_header_path = questions_path + q.qid + "/header.hpp";
                std::string question_tail_path = questions_path + q.qid + "/tail.cpp";

                FileUtile::ReadFile(question_desc_path, &q.desc);
                FileUtile::ReadFile(question_header_path, &q.header);
                FileUtile::ReadFile(question_tail_path, &q.tail);

                questions_.insert({q.qid, q});
            }
            in.close();
            MYLOG_INFO("加载题库成功.........");
            return true;
        }

        bool GetAllQuestions(std::vector<question> *out)
        {
            if (questions_.empty())
            {
                MYLOG_INFO("用户获取所有题目失败");
                return false;
            }
            for (const auto &q : questions_)
            {
                out->push_back(q.second);
            }
            return true;
        }

        bool GetOneQuestion(const std::string &num, question *q)
        {
            auto it = questions_.find(num);
            if (it == questions_.end())
            {
                MYLOG_INFO("用户获取题目失败 题号:%s", num.c_str());
                return false;
            }
            *q = it->second;
            return true;
        }

        // 保存判题结果到 Redis（带过期时间）
        bool SetResult(const std::string &taskid, const std::string &result_json, int ttl = 300)
        {
            std::lock_guard<std::mutex> lock(redis_mtx_);
            try
            {
                redis_client_.setex(taskid, ttl, result_json);
                redis_client_.sync_commit();
                return true;
            }
            catch (...)
            {
                MYLOG_ERROR("Redis 设置判题结果失败 taskid: %s", taskid.c_str());
                return false;
            }
        }

        // 从 Redis 中获取判题结果
        std::string GetResult(const std::string &taskid)
        {
            std::lock_guard<std::mutex> lock(redis_mtx_);
            try
            {
                auto future = redis_client_.get(taskid);
                redis_client_.sync_commit();
                auto reply = future.get();
                if (reply.is_string())
                {
                    return reply.as_string();
                }
            }
            catch (...)
            {
                MYLOG_ERROR("Redis 获取判题结果失败 taskid: %s", taskid.c_str());
            }
            return "";
        }

    private:
        void InitRedis()
        {
            std::lock_guard<std::mutex> lock(redis_mtx_);
            try
            {
                redis_client_.connect("127.0.0.1", 6379);
                redis_client_.ping(); // 尝试发送一个 ping 命令看能不能连通
                redis_client_.sync_commit();
                MYLOG_INFO("Redis 同步连接成功");
            }
            catch (const std::exception &e)
            {
                MYLOG_FATAL("Redis 同步连接失败: %s", e.what());
            }
        }

    private:
        std::unordered_map<std::string, question> questions_;
        cpp_redis::client redis_client_;
        std::mutex redis_mtx_;
    };
};
