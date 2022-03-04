#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Singleton.hpp"

#define REGISTER_COMPONENT_CLASS_IN_ENGINE_SYSTEM(...) \
    registerComponentClass(__VA_ARGS__::getClassIdStatic());

#define REGISTER_ENGINE_SYSTEM(engineSystem) \
    EngineSystemsManager::getInstance().registerEngineSystem(engineSystem);

#define ADD_COMPONENT_TO_ENGINE_SYSTEM(EngineSystemComponent) \
    EngineSystemsManager::getInstance().addComponentToEngineSystem(EngineSystemComponent);

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
private:
    std::set<ClassId> mAcceptedEngineSystemComponentClasses;

public:
    void registerComponentClass(ClassId classId)
    {
        mAcceptedEngineSystemComponentClasses.insert(classId);
    }

    bool isComponentClassAccepted(ClassId classId)
    {
        return mAcceptedEngineSystemComponentClasses.find(classId) != mAcceptedEngineSystemComponentClasses.end();
        ;
    }

    virtual void init();

    virtual void addComponent(IEngineSystemComponent* component);
};

class EngineSystemsManager : public Singleton<EngineSystemsManager>
{
private:
    std::list<IEngineSystem *> mEngineSystems;

public:
    void addComponentToEngineSystem(IEngineSystemComponent* component);

    void registerEngineSystem(IEngineSystem *engineSystem)
    {
        mEngineSystems.push_back(engineSystem);
    }

    const std::list<IEngineSystem *> &getEngineSystems() const
    {
        return mEngineSystems;
    }
};