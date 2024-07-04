#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Scene/GameObject.hpp"

class GameObjectsManager;

class GameObjectHandler
{
public:
    GameObjectHandler() = default;
    GameObjectHandler(ClassId id, Slot slot, GameObjectsManager* gameObjectsManager)
    {
        mClassId = id;
        mSlot = slot;
        mGameObjectsManager = gameObjectsManager;
        CHECK_MSG(isValid(), "Invalid handler!");
    }

    GameObjectHandler(ClassId id, Slot slot, const GameObjectsManager* gameObjectsManager): GameObjectHandler(id, slot, const_cast<GameObjectsManager*>(gameObjectsManager))
    {
    }

    ~GameObjectHandler()
    {
        reset();
    }

    template<class T> T_EXTENDS(T, GameObject)
    Ptr<T> get() const
    {
        return Ptr<T>::cast(getGameObject());
    }

    Ptr<GameObject> operator->() const { return getGameObject(); }

    Ptr<GameObject> getGameObject() const;
    bool isValid() const { return mGameObjectsManager && mClassId > 0 && mSlot.isValid(); }
    bool operator==(const GameObjectHandler& other) const
	{
		return
         mGameObjectsManager == other.mGameObjectsManager &&
         mClassId == other.mClassId &&
         mSlot.getSlot() == other.mSlot.getSlot();
	}

    void reset()
    {
        mSlot.reset();
        mClassId = 0;
        mGameObjectsManager = nullptr;
    }

public:
    Slot mSlot;
    ClassId mClassId = 0;
    GameObjectsManager* mGameObjectsManager = nullptr;
};

class GameObjectsManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    template<class T> T_EXTENDS(T, GameObject)
    GameObjectHandler requestGameObject()
    {
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mGameObjectsArrays.contains(id))
        {
            mGameObjectsArrays.emplace(id, smInitialGameObjects);
        }

        if(mGameObjectsArrays.at(id).mSlotsManager.isEmpty())
        {
            mGameObjectsArrays.at(id).mSlotsManager.increaseSize(smInitialGameObjects);
            mGameObjectsArrays.at(id).mGameObjects.resize(mGameObjectsArrays.at(id).mSlotsManager.getSize());
        }

        GameObjectHandler gameObjectHandler(id, mGameObjectsArrays.at(id).mSlotsManager.requestSlot(), this);
        u32 slot = gameObjectHandler.mSlot.getSlot();
        if(mGameObjectsArrays.at(id).mGameObjects[slot].isValid())
        {
            mGameObjectsArrays.at(id).mGameObjects[slot]->onRecycle();
        }
        else
        {
            mGameObjectsArrays.at(id).mGameObjects[slot] = OwnerPtr<GameObject>::moveCast(OwnerPtr<T>::newObject());
        }

        mPtrToHandler.emplace(mGameObjectsArrays.at(id).mGameObjects[slot], gameObjectHandler);

        return gameObjectHandler;
    }

    void removeGameObject(GameObjectHandler& gameObjectHandler)
    {
        u32 slot = gameObjectHandler.mSlot.getSlot();
        ClassId id = gameObjectHandler.mClassId;

        if(mGameObjectsArrays.contains(id))
        {
            mPtrToHandler.erase(mGameObjectsArrays.at(id).mGameObjects[slot]);
            
            // mGameObjectsArrays.at(gameObjectHandler.mClassId).mGameObjects[slot].invalidate();
            mGameObjectsArrays.at(id).mSlotsManager.freeSlot(gameObjectHandler.mSlot);
        }
        gameObjectHandler.reset();
    }

    void removeGameObject(Ptr<GameObject> gameObject)
    {
        // NOTE: copy!
        GameObjectHandler gameObjectHandler = mPtrToHandler.at(gameObject);
        removeGameObject(gameObjectHandler);
    }

    Ptr<GameObject> getGameObject(GameObjectHandler gameObjectHandler) const
    {
        u32 slot = gameObjectHandler.mSlot.getSlot();
        return mGameObjectsArrays.at(gameObjectHandler.mClassId).mGameObjects[slot];
    }

    Ptr<GameObject> getGameObjectFromSlot(ClassId classId, const Slot& slot) const
    {
        return mGameObjectsArrays.at(classId).mGameObjects[slot.getSlot()];
    }

private:
    class GameObjectsArray
    {
    public:
        GameObjectsArray(u32 reservedGameObjects)
        {
            mGameObjects.resize(reservedGameObjects);
            mSlotsManager.init(reservedGameObjects);
        }
        std::vector<OwnerPtr<GameObject>> mGameObjects;
        SlotsManager mSlotsManager;
    };

    std::unordered_map<ClassId, GameObjectsArray> mGameObjectsArrays;
    std::unordered_map<Ptr<GameObject>, GameObjectHandler> mPtrToHandler;

    inline static const u32 smInitialGameObjects = 3000;
};
REGISTER_CLASS(GameObjectsManager);