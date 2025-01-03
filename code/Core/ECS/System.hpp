#pragma once

#include "Core/Std.hpp"
#include "Core/Memory/Singleton.hpp"
#include "Core/ECS/ComponentHandler.hpp"

class System: public IEventObject
{
public:
    virtual ~System() = default;
    virtual void init() { };
    virtual void update() { };
    virtual void terminate() { };
    void registerComponentClass(ClassId classId);
    bool isComponentClassAccepted(ClassId classId) const;
    virtual void addSystemComponent(const ComponentHandler& component);
    virtual void removeSystemComponent(const ComponentHandler& component);
    void checkSystemComponent(const ComponentHandler& component) const;

private:
    std::unordered_set<ClassId> mAcceptedSystemComponentClasses;
};
REGISTER_CLASS(System);