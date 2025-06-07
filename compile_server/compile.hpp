#pragma once

#include "../comm/myutile/BasePathUtil.hpp"
#include "../comm/myutile/ojlog.hpp"
#include "../comm/myutile/CommnUtil.hpp"
#include <sys/wait.h>
#include <memory>
#include <sys/types.h>
#include <libgen.h>   // dirname
#include <unistd.h>   // access
#include <fcntl.h>
#include <cstring>    // strerror
#include <errno.h>    // errno
#include <filesystem> // C++17
#include <string>
#include <vector>
class Compiler
{
protected:
    virtual std::vector<std::string> CompileCommand(const std::string &file_name) = 0;

    std::shared_ptr<BasePathUtil> pathUtil_;

public:
    // 必须传std::move后的值
    Compiler(std::shared_ptr<BasePathUtil> pathUtil)
        : pathUtil_(pathUtil)
    {
        if(pathUtil_ == nullptr)
        {
            MYLOG_FATAL("编译器pathUtil为空abort");
        }
    }
    virtual ~Compiler() = default;
    // 核心抽象点：返回要执行的命令及参数

    bool Compile(const std::string &file_name)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            int errorfd = -1;

            std::string errorPath = pathUtil_->buildCompilerError(file_name);
            std::string dirPath = errorPath.substr(0, errorPath.find_last_of('/')); // 提取目录路径
        
            // 使用 C++17 的 <filesystem> 判断并创建目录
            namespace fs = std::filesystem;
            if (!fs::exists(dirPath))
            {
                try
                {
                    fs::create_directories(dirPath);
                }
                catch (const fs::filesystem_error &e)
                {
                    MYLOG_ERROR("目录创建失败: %s", e.what());
                    return false;
                }
            }

            // 打开文件标准错误
            errorfd = ::open(errorPath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (errorfd < 0)
            {
                MYLOG_ERROR("打开 error 文件失败: %s", strerror(errno));
                return false;
            }


            dup2(errorfd, 2);

            std::vector<std::string> cmd = CompileCommand(file_name);
            // 转为 const char* 数组
            std::vector<const char *> argv;
            for (const auto &s : cmd)
                argv.push_back(s.c_str());
            argv.push_back(nullptr);
            
       
            execvp(argv[0], const_cast<char *const *>(argv.data()));
            MYLOG_ERROR("%s 编译器执行失败", argv[0]);
            exit(2);
        }
        else if (pid < 0)
        {
            MYLOG_ERROR("fork失败: %s", strerror(errno));
            return false;
        }
        else
        {
            int status = 0;
            waitpid(pid, &status, 0);
            return true;
        }
    }
};
