
#pragma once
#include "compile.hpp"
#include "compilesFactory.hpp"
// cpp 编译器
class CppCompiler : public Compiler
{
public:
    CppCompiler(std::shared_ptr<BasePathUtil> util)
        : Compiler(util) {}
    std::vector<std::string> CompileCommand(const std::string &file_name) override
    {
        return {
            "g++",
            "-o", pathUtil_->buildExe(file_name),
            pathUtil_->buildSrc(file_name),
            "-std=c++17"};
    }

private:
    static bool isRegist_;
};
// cpp 注册方法在第一次实例化时就会注册进工厂
inline bool CppCompiler::isRegist_ = []() -> bool
{
    CompilesFactory::Regist(Language::CPP, [](std::shared_ptr<BasePathUtil> pathUtil)
                            {
                                return std::make_unique<CppCompiler>(pathUtil);
                            });

    return true;
}();

// java 编译器
class JavaCompiler : public Compiler
{
public:
    JavaCompiler(std::shared_ptr<BasePathUtil> util)
        : Compiler(util) {}

    std::vector<std::string> CompileCommand(const std::string &file_name) override
    {
        return {
            "javac",
            pathUtil_->buildSrc(file_name)};
    }

private:
    static bool isRegist_;
};
// Java注册器
inline bool JavaCompiler::isRegist_ = []() -> bool
{
    CompilesFactory::Regist(Language::JAVA, [](std::shared_ptr<BasePathUtil> pathUtil)
                            { return std::make_unique<JavaCompiler>(pathUtil); });
    return true;
}();
// go编译器
class GoCompiler : public Compiler
{
public:
    GoCompiler(std::shared_ptr<BasePathUtil> util)
        : Compiler(util) {}

    std::vector<std::string> CompileCommand(const std::string &file_name) override
    {
        return {
            "go",
            "build",
            "-o",
            pathUtil_->buildExe(file_name),
            pathUtil_->buildSrc(file_name)};
    }

private:
    static bool isRegist_;
};
// go 注册器
inline bool GoCompiler::isRegist_ = []() -> bool
{
    CompilesFactory::Regist(Language::GO, [](std::shared_ptr<BasePathUtil> pathUtil)
                            { return std::make_unique<GoCompiler>(pathUtil); });
    return true;
}();
