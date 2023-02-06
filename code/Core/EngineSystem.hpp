#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Singleton.hpp"
#include "Core/ObjectBase.hpp"

#define REGISTER_COMPONENT_CLASS_IN_ENGINE_SYSTEM(...) \
    registerComponentClass(__VA_ARGS__::getClassIdStatic());

#define REGISTER_ENGINE_SYSTEM(engineSystem) \
    EngineSystemsManager::getInstance().registerEngineSystem(engineSystem);

#define ADD_COMPONENT_TO_ENGINE_SYSTEM(engineSystemComponent) \
    EngineSystemsManager::getInstance().addComponentToEngineSystem(engineSystemComponent);

class IEngineSystemComponent: public ObjectBase
{
    GENERATE_METADATA(IEngineSystemComponent)

private:
    bool mAlreadyAddedToEngine = false;
public:
	GET_SET(AlreadyAddedToEngine)
};

class IEngineSystem: public ObjectBase
{
    GENERATE_METADATA(IEngineSystem)

public:
    void registerComponentClass(ClassId classId);
    bool isComponentClassAccepted(ClassId classId);
    virtual void init();
    virtual void addComponent(Ptr<IEngineSystemComponent> component);

private:
    std::set<ClassId> mAcceptedEngineSystemComponentClasses;
};

class EngineSystemsManager : public Singleton<EngineSystemsManager>
{
public:
    void addComponentToEngineSystem(Ptr<IEngineSystemComponent> component);
    void registerEngineSystem(Ptr<IEngineSystem> engineSystem);

private:
    std::list<Ptr<IEngineSystem>> mEngineSystems;
};
