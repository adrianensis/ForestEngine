#pragma once

#include "Core/Core.hpp"
#include "Core/System/SystemsDependencyInjection.hpp"

NS_BEGIN(System)
class System
{
public:
    virtual ~System() = default;
    virtual void terminate() { };

    void injectSystemDependencies(const SystemsDependencyInjection& systemsDependencyInjection)
    {
        mSystemsDI = systemsDependencyInjection;
    }

protected:
    SystemsDependencyInjection mSystemsDI;
public:
    RGET(SystemsDI)
};
NS_END