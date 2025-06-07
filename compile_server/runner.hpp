#pragma once
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <cstring>

#include "../comm/myutile/BasePathUtil.hpp"
#include "../comm/myutile/ojlog.hpp"
#include "../comm/myutile/CommnUtil.hpp"

class Runner
{
protected:
    virtual std::vector<std::string> RunCommand(const std::string &file_name) = 0;
    std::shared_ptr<BasePathUtil> pathUtil_;

public:
    Runner(std::shared_ptr<BasePathUtil> util)
        : pathUtil_(util)
    {
    }

    // 需要出题者传入 最大内存限制 和 cpu占用限制
    std::pair<int, int> Run(const std::string &filename, int cpu_limit, int mem_limit)
    {
        std::string execute = pathUtil_->buildExe(filename);
        std::cout << execute << std::endl;
        std::string _stdin = pathUtil_->StdIn(filename);
        std::string _stdout = pathUtil_->StdOut(filename);
        std::string _stderror = pathUtil_->StdError(filename);
        std::string ResultFile = pathUtil_->ResultFile(filename);

        umask(0);
        // 打开标准输出 标准输入和标准错误和结果文件

        int infd = open(_stdin.c_str(), O_CREAT | O_RDONLY, 0644);
        int outfd = open(_stdout.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
        int errorfd = open(_stderror.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
        int refd = open(ResultFile.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (infd < 0 || outfd < 0 || errorfd < 0)
        {
            MYLOG_FATAL("运行所需文件打开失败 errno: %d error message : %s", errno, strerror(errno));
            return {-1, -1};
        }
        pid_t pid = fork();
        if (pid < 0)
        {
            ::close(infd);
            ::close(outfd);
            ::close(errorfd);
            MYLOG_FATAL("创建子进程失败 errno: %d error message %s", errno, strerror(errno));
            return {-1, -1};
        }
        else if (pid == 0)
        {
            // SetProcLimit(cpu_limit, mem_limit + 256 * 1024);

            dup2(infd, 0);
            dup2(outfd, 1);
            dup2(errorfd, 2);
            // 这里是一个多态函数应该由子类进行实现。
            std::vector<std::string> cmd = RunCommand(filename);

            std::vector<const char *> argv = parseCmd(cmd, ResultFile);

            execvp(argv[0], const_cast<char *const *>(argv.data()));

            MYLOG_ERROR("%s 运行器执行失败", argv[0]);
            exit(2);
        }
        else
        {
            ::close(infd);
            ::close(outfd);
            ::close(errorfd);

            int status = 0;
            waitpid(pid, &status, 0);

            int retcode = (status >> 8) & 0xFF;
            int signal = status & 0x7F;

            MYLOG_INFO(" run done retcode: %d signal: %d", retcode, signal);

            return std::make_pair(retcode, signal);
        }
    }

private:
    void SetProcLimit(int cpu_limit, int mem_limit)
    {
        struct rlimit cpu_rlimit;
        struct rlimit mem_rlimit;
        cpu_rlimit.rlim_max = RLIM_INFINITY;
        cpu_rlimit.rlim_cur = cpu_limit;
        mem_rlimit.rlim_max = RLIM_INFINITY;
        mem_rlimit.rlim_cur = mem_limit * 1024; // KB 转 byte

        setrlimit(RLIMIT_CPU, &cpu_rlimit);
        setrlimit(RLIMIT_AS, &mem_rlimit);
    }

    std::vector<const char *> parseCmd(const std::vector<std::string> &cmd, const std::string &ResultFile)
    {
        std::vector<const char *> argv;
        for (const auto &s : cmd)
            argv.push_back(s.c_str());

        argv.push_back(ResultFile.c_str()); // 加上ResultFile参数

        argv.push_back(nullptr); // 末尾加nullptr结束标志

        return argv;
    }
};
