#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Singleton.hpp"
#include "Core/ObjectBase.hpp"

#ifdef CPP_INCLUDE
#include "Core/EngineSystem.hpp"
#endif

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
    CPP void registerComponentClass(ClassId classId)
    {
        mAcceptedEngineSystemComponentClasses.insert(classId);
    }

    CPP bool isComponentClassAccepted(ClassId classId)
    {
        return mAcceptedEngineSystemComponentClasses.find(classId) != mAcceptedEngineSystemComponentClasses.end();
    }

    CPP virtual void init()
    {
        REGISTER_ENGINE_SYSTEM(this);
    }

    CPP virtual void addComponent(Ptr<IEngineSystemComponent> component)
    {
        component.get().setAlreadyAddedToEngine(true);
    }

private:
    std::set<ClassId> mAcceptedEngineSystemComponentClasses;
};

class EngineSystemsManager : public Singleton<EngineSystemsManager>
{
public:
    CPP void addComponentToEngineSystem(Ptr<IEngineSystemComponent> component)
    {
        if (component && !component.get().getAlreadyAddedToEngine())
        {
            ClassId componentClassId = component.get().getClassId();
            bool added = false;
            FOR_LIST_COND(itEngineSystem, mEngineSystems, !added)
            {
                IEngineSystem *sub = (*itEngineSystem);
                if (sub->isComponentClassAccepted(componentClassId))
                {
                    sub->addComponent(component);
                    added = true;
                }
            }
        }
    }


    CPP void registerEngineSystem(IEngineSystem *engineSystem)
    {
        if(!CONTAINS(mEngineSystems, engineSystem))
        {
            mEngineSystems.push_back(engineSystem);
        }
    }

    CPP const std::list<IEngineSystem *>& getEngineSystems() const
    {
        return mEngineSystems;
    }
    
private:
    std::list<IEngineSystem *> mEngineSystems;

};