#pragma once

#include "Core/Std.hpp"
#include "Core/Memory/Singleton.hpp"
#include "Core/Events/Event.hpp"

class System: public EnablePtrToThis, public IEventObject
{
public:
    virtual ~System() = default;
    virtual void init() { };
    virtual void terminate() { };
};
REGISTER_CLASS(System);