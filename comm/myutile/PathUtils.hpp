
#pragma once
#include "BasePathUtil.hpp"
#include "PathUtilsFactory.hpp"

class CppPathUtil : public BasePathUtil{
public:
    CppPathUtil(const PathContext & ctx)
    :BasePathUtil(ctx)
    {}
    std::string buildSrc(const std::string& filename) const override {
        return AddSuffix(filename, ".cpp");
    }
    std::string buildExe(const std::string& filename) const override {
        return AddSuffix(filename, ".exe");
    }
    std::string buildCompilerError(const std::string& filename) const override {
        return AddSuffix(filename, ".compiler_err");
    }

private:
    // 注册标记，确保只执行一次
    static bool registered_;
};

inline bool CppPathUtil::registered_ = []() {
    PathUtilsFactory::Register(Language::CPP, [](const PathContext& ctx) -> std::shared_ptr<BasePathUtil> {
        return std::make_shared<CppPathUtil>(ctx);
    });
    return true;
}();


class GoPathUtil : public BasePathUtil
{
public:
    GoPathUtil(const PathContext &ctx)
        : BasePathUtil(ctx)
    {
    }
    std::string buildSrc(const std::string &filename) const override
    {
        return AddSuffix(filename, ".go");
    }
    std::string buildExe(const std::string &filename) const override
    {
        return AddSuffix(filename, ".exe"); // Go编译后一般是可执行文件，可以加后缀或不加
    }
    std::string buildCompilerError(const std::string &filename) const override
    {
        return AddSuffix(filename, ".compiler_err");
    }

private:
    static bool registered_;
};

inline bool GoPathUtil::registered_ = []()
{
    PathUtilsFactory::Register(Language::GO, [](const PathContext &ctx)
                              { return std::make_shared<GoPathUtil>(ctx); });
    return true;
}();



class JavaPathUtil : public BasePathUtil
{
public:
    JavaPathUtil(const PathContext &ctx)
        : BasePathUtil(ctx)
    {
    }
    std::string buildSrc(const std::string &filename) const override
    {
        return AddSuffix(filename, ".java");
    }
    // Java 不需要可执行文件，这里返回空串或者抛异常也可以
    std::string buildExe(const std::string &filename) const override
    {
        return AddSuffix(filename,"");
    }
    std::string buildCompilerError(const std::string &filename) const override
    {
        return AddSuffix(filename, ".compiler_err");
    }

private:
    static bool registered_;
};

inline bool JavaPathUtil::registered_ = []()
{
    PathUtilsFactory::Register(Language::JAVA, [](const PathContext& ctx)
                              { return std::make_shared<JavaPathUtil>(ctx); });
    return true;
}();
