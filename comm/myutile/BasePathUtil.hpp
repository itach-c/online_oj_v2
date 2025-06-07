#pragma once
#include <string>

const std::string tmpPath = "./tmp/";

enum class Language
{
    CPP,
    JAVA,
    GO,
};



std::string LanguageToPathSuffix(Language lang)
{
    switch (lang)
    {
    case Language::CPP:
        return "cpp";
    case Language::JAVA:
        return "java";
    case Language::GO:
        return "go";
    default:
        return "unknown";
    }
}
struct PathContext
{
    PathContext(const std::string judgeid,const std::string& lang = "")
    :judge_id(judgeid),language(lang)
    {
    }

   void SetLanguage(Language lang)
    {
      language =  LanguageToPathSuffix(lang);
    }
    std::string judge_id; // e.g., "judge01"
    std::string language; // e.g., "cpp", "java", "python"
};
class BasePathUtil
{
protected:
    PathContext context_;

protected:
    std::string AddSuffix(const std::string &filename, const std::string &suffix) const
    {
        std::string prefix = "./data/" + context_.judge_id + "/" + context_.language + "/tmp/";
        return prefix + filename + suffix;
    }

public:
    BasePathUtil(const PathContext &ctx)
        : context_(ctx) {}

    virtual std::string buildSrc(const std::string &filename) const = 0;
    virtual std::string buildExe(const std::string &filename) const = 0;
    virtual std::string buildCompilerError(const std::string &filename) const = 0;

    // 通用的实现：可直接复用
    virtual std::string StdIn(const std::string &filename) const
    {
        return AddSuffix(filename, ".stdin");
    }
    virtual std::string StdOut(const std::string &filename) const
    {
        return AddSuffix(filename, ".stdout");
    }
    virtual std::string StdError(const std::string &filename) const
    {
        return AddSuffix(filename, ".stderr");
    }
    virtual std::string ResultFile(const std::string &filename) const
    {
        return AddSuffix(filename, ".result");
    }

    virtual ~BasePathUtil() = default;
};
