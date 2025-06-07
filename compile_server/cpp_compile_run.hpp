#pragma once
#include "runner.hpp"
#include "jsoncpp/json/json.h"
#include "../comm/myutile/ojlog.hpp"
#include "../comm/myutile/utile.hpp"
#include "compiler.hpp"
#include "runner.hpp"
#include "../comm/message.pb.h"

namespace ns_compile_run
{
    using namespace ns_utile;
    using namespace ns_compile;
    using namespace ns_runner;

    class CompileAndRun
    {

    public:
        /*** **************************
         * 输入:
         * .code  用户代码
         * .input 用户自己的输入先不处理
         * .cpulimit
         * .memlimit
         *
         * 输出
         * status 状态码
         * reson 请求原因
         * 选填字段
         * stdout 运行结果
         * stderror 程序完成的错误结果
         *
         *
         */

        static bool removeTempFile(const std::string &file_name)
        {
            namespace fs = std::filesystem;
            std::string path = pathUtile::AddSuffix(file_name, ""); // 不加 *，直接遍历目录
            try
            {
                for (const auto &entry : fs::directory_iterator(path))
                {
                    fs::remove_all(entry); // 删除文件或整个子目录
                }
                return true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "removeTempFile error: " << e.what() << std::endl;
                return false;
            }
        }
        static std::string CodetoDesc(int code, const std::string &file_name)
        {

            switch (code)
            {
            case 0:
            {
                return "compile and run success";
            }
            case -1:
            {
                return "the code is empty";
            }
            case -2:
            {
                return "unknown";
            }
            case -3:
            {
            }
            case SIGXCPU:
            {
                return "cpu limit";
            }
            case SIGABRT:
            {
                return "mem limit";
            }
            default:
            {
                return "unknown";
            }
            }
        }
        
    };
}