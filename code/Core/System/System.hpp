#pragma once

#include "Core/Core.hpp"

NS_BEGIN(System)
class System
{
public:
    virtual ~System() = default;
    virtual void terminate() { };
};
NS_END