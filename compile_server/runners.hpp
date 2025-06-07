
#pragma once
#include "runner.hpp"
#include "runnersFactory.hpp"
//此文件必须被用处显示包含,不然无法向工厂中注册方法
// C++ Runner
// CppRunner.hpp
class CppRunner : public Runner
{
public:
    CppRunner(std::shared_ptr<BasePathUtil> util) : Runner(util) {}

protected:
    std::vector<std::string> RunCommand(const std::string &file_name) override
    {
        std::string exe = pathUtil_->buildExe(file_name);
        return {exe};
    }

private:
    static bool isRegist_;
};

bool CppRunner::isRegist_ = []() -> bool {
    RunnerFactory::Register(Language::CPP, [](std::shared_ptr<BasePathUtil> util) {
        return std::make_unique<CppRunner>(util);
    });
    return true;
}();


// JavaRunner.hpp
class JavaRunner : public Runner
{
public:
    JavaRunner(std::shared_ptr<BasePathUtil> util) : Runner(util) {}

protected:
    std::vector<std::string> RunCommand(const std::string &file_name) override
    {
        std::string classname = pathUtil_->buildExe("Main");
        std::string tmpDir =  classname.substr(0,classname.rfind("/"));
        return {"java", "-cp",tmpDir,"Main"};
    }

private:
    static bool isRegist_;
};

bool JavaRunner::isRegist_ = []() -> bool {
    RunnerFactory::Register(Language::JAVA, [](std::shared_ptr<BasePathUtil> util) {
        return std::make_unique<JavaRunner>(util);
    });
    return true;
}();


// GoRunner.hpp
class GoRunner : public Runner
{
public:
    GoRunner(std::shared_ptr<BasePathUtil> util) : Runner(util) {}

protected:
    std::vector<std::string> RunCommand(const std::string &file_name) override
    {
        std::string exe = pathUtil_->buildExe(file_name);
        return {exe};
    }

private:
    static bool isRegist_;
};

bool GoRunner::isRegist_ = []() -> bool {
    RunnerFactory::Register(Language::GO, [](std::shared_ptr<BasePathUtil> util) {
        return std::make_unique<GoRunner>(util);
    });
    return true;
}();
