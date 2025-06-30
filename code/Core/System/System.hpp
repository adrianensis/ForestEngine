#pragma once

#include "Core/Std.hpp"
#include "Core/Memory/Singleton.hpp"
#include "Core/Events/Event.hpp"

class System: public EnableWeakPtrToThis, public Core::IEventObject
{
public:
    virtual ~System() override = default;
    virtual void init() { };
    virtual void terminate() { };
};
REGISTER_CLASS(System);