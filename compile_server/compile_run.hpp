#pragma once
#include "../comm/myutile/CommnUtil.hpp"
#include "../comm/myutile/PathUtils.hpp"
#include "../comm/myutile/PathUtilsFactory.hpp"

#include "compilers.hpp"
#include "compilesFactory.hpp"

#include "runners.hpp"
#include "runnersFactory.hpp"

#include <memory>

enum class StatusCode : int
{
    COMPILE_ERROR = -1, // 编译错误
    SIGNAL_KILLED = -2, // 被信号杀死
    ALL_PASSED = 0,     // 全部测试用例通过
    PARTIAL_PASS = 1,   // 通过部分测试用例
    UNKOWN = 2
};
struct Request
{
public:
    std::string code = "";  // 代码
    std::string input = ""; // 输入
    Language lang;          // 语言
    int cpu_limit;
    int mem_limit;
};
struct Response
{
public:
    StatusCode status = StatusCode::UNKOWN; // 状态码 -1表示编译错误,-2是被信号所杀 0是通过所有测试用例,1是通过部分测试用例
    std::string reason = "";                // 原因
    std::string stdout_data = "";           // 标准输出
    std::string stderr_data;                // 标准错误
    int passnum = -1;                       // 通过数
    int totalnum = -1;                      // 总的测试样例
    std::string last_error_input = "";      // 最后一次测试用例的输入;
};

class CompileAndRun
{
public:
    static PathUtilsFactory pathUtilsFactory_;

    static CompilesFactory compilesFactory_;

    static RunnerFactory runnerFactory_;
    static void Run(PathContext &ctx, const Request &req, Response *resp)
    {
        // 生成唯一源文件名
        ctx.SetLanguage(req.lang);
        const std::string file_name = FileUtile::unique_file_name();
        const Language lang = req.lang;

        auto pathUtil = pathUtilsFactory_.Create(lang, ctx);
        std::string tmpDir = pathUtil->buildSrc(file_name);
        tmpDir = tmpDir.substr(0, tmpDir.rfind("/"));

        // 准备源文件
        if (!PrepareSourceFile(pathUtil.get(), file_name, req.code))
        {
            return;
        }
        // 编译源文件
        if (!CompileSource(pathUtil, file_name, lang, resp))
        {
           // removeTempFile(tmpDir);
            return;
        }
        // 运行程序
        if (!ExecuteAndParseResult(pathUtil, file_name, req.cpu_limit, req.mem_limit, lang, resp))
        {
         //   removeTempFile(tmpDir);
            return;
        }

      //  removeTempFile(tmpDir);
    }

private:
    // 源文件的写入
    static bool PrepareSourceFile(BasePathUtil *pathUtil, const std::string &file_name, const std::string &code)
    {
        namespace fs = std::filesystem;
        std::string srcPath = pathUtil->buildSrc(file_name);
        std::string dirPath = srcPath.substr(0, srcPath.find_last_of('/'));

        if (!fs::exists(dirPath))
        {
            try
            {
                fs::create_directories(dirPath);
            }
            catch (const fs::filesystem_error &e)
            {
                MYLOG_ERROR("创建目录失败: %s", e.what());
                return false;
            }
        }

        if (!FileUtile::WriteFile(srcPath, code))
        {
            MYLOG_ERROR("写入源文件失败");
            return false;
        }

        return true;
    }

    // 编译源文件
    static bool CompileSource(std::shared_ptr<BasePathUtil> pathUtil, const std::string &file_name, Language lang, Response *resp)
    {
        auto compile = compilesFactory_.Create(lang, pathUtil);
        if (!compile->Compile(file_name))
        {
            resp->status = StatusCode::COMPILE_ERROR;

            std::string compile_err;
            FileUtile::ReadFile(pathUtil->buildCompilerError(file_name), &compile_err);
            resp->reason = std::move(compile_err);

            return false;
        }

        MYLOG_INFO("编译成功");
        return true;
    }

    // 运行并且解析结果
    static bool ExecuteAndParseResult(std::shared_ptr<BasePathUtil> pathUtil, const std::string &file_name,
                                      int cpu_limit, int mem_limit, Language lang, Response *resp)
    {
        auto runner = runnerFactory_.Create(lang, pathUtil);
        auto [exit_code, signal] = runner->Run(file_name, cpu_limit, mem_limit);

        if (signal > 0)
        {
            resp->status = StatusCode::SIGNAL_KILLED;
            resp->reason = strsignal(signal);
            return false;
        }

        if (exit_code == 0)
        {
            resp->status = StatusCode::ALL_PASSED;
        }
        else
        {
            resp->status = StatusCode::PARTIAL_PASS;
            std::string result;
            if (FileUtile::ReadFile(pathUtil->ResultFile(file_name), &result))
            {
                std::vector<std::string> parts;
                if (splitIntoThree(result, &parts) && parts.size() == 3)
                {
                    resp->passnum = std::stoi(parts[0]);
                    resp->totalnum = std::stoi(parts[1]);
                    resp->last_error_input = parts[2];
                }
            }
        }

        FileUtile::ReadFile(pathUtil->StdOut(file_name), &resp->stdout_data);
        FileUtile::ReadFile(pathUtil->StdError(file_name), &resp->stderr_data);

        return true;
    }

    static bool splitIntoThree(const std::string &result, std::vector<std::string> *out)
    {
        if (!out)
            return false; // 指针空检查

        std::istringstream iss(result);
        std::string token;

        int count = 0;
        while (count < 3 && iss >> token)
        {
            out->push_back(token);
            count++;
        }

        return count == 3; // 是否拆出3个字符串
    }
    static bool removeTempFile(const std::string &file_name)
    {

        namespace fs = std::filesystem;
        // 不加 *，直接遍历目录
        try
        {
            for (const auto &entry : fs::directory_iterator(file_name))
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
};

PathUtilsFactory CompileAndRun::pathUtilsFactory_;
CompilesFactory CompileAndRun::compilesFactory_;
RunnerFactory CompileAndRun::runnerFactory_;