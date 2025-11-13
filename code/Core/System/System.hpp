#pragma once

#include "Core/Core.hpp"
#include "Core/Event/Event.hpp"

NS_BEGIN(System)
class System: public Event::IEventObject
{
public:
    virtual ~System() = default;
    virtual void init() { };
    virtual void terminate() { };
};
NS_END