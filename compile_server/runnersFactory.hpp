#pragma once
#include <functional>
#include <unordered_map>
#include <memory>
#include "runner.hpp"
#include "../comm/myutile/BasePathUtil.hpp"

class RunnerFactory
{
public:
    using Creator = std::function<std::unique_ptr<Runner>(std::shared_ptr<BasePathUtil> pathUtil)>;

    static std::unique_ptr<Runner> Create(Language lang, std::shared_ptr<BasePathUtil> util)
    {
        auto &Map = GetMap();
        auto it = Map.find(lang);
        if (it == Map.end())
        {
            return nullptr;
        }
        return it->second(util);
    }

    static void Register(Language lang, Creator creator)
    {
        GetMap()[lang] = std::move(creator);
    }

private:
    static std::unordered_map<Language, Creator> &GetMap()
    {
        static std::unordered_map<Language, Creator> instance;
        return instance;
    }
};
