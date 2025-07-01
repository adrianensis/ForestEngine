#pragma once

#include "Core/Std.hpp"
#include "Core/Memory/Singleton.hpp"
#include "Engine/Events/Event.hpp"

NS_BEGIN(Core)
class System: public EnableWeakPtrToThis, public Event::IEventObject
{
public:
    virtual ~System() override = default;
    virtual void init() { };
    virtual void terminate() { };
};
REGISTER_CLASS(System);
NS_END