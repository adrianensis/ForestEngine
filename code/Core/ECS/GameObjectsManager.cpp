#include "Core/ECS/GameObjectsManager.hpp"
#include "Scene/GameObject.hpp"

void GameObjectsManager::init() 
{
}

void GameObjectsManager::terminate() 
{
}

Ptr<GameObject> GameObjectHandler::getGameObject() const
{
    CHECK_MSG(isValid(), "Invalid handler!");
    GameObjectHandler gameObjectHandler = *this;
    return mGameObjectsManager->getGameObject(gameObjectHandler);
}