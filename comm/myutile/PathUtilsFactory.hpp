#pragma once
#include "BasePathUtil.hpp"
#include <functional>
#include <unordered_map>
#include <memory>


class PathUtilsFactory
{
public:
    using Creator = std::function<std::shared_ptr<BasePathUtil>(PathContext ctx)>;

    // 注册语言对应的构造函数
    static void Register(Language lang, Creator creator)
    {
        auto &registry = GetRegistry();
        registry[lang] = std::move(creator);
    }

    // 创建实例
    static std::shared_ptr<BasePathUtil> Create(Language lang,PathContext ctx)
    {
        auto &registry = GetRegistry();
        auto it = registry.find(lang);
        if (it != registry.end())
        {
            return it->second(ctx);
        }
        return nullptr;
    }

private:
    static std::unordered_map<Language, Creator> &GetRegistry()
    {
        static std::unordered_map<Language, Creator> registry;
        return registry;
    }
};
