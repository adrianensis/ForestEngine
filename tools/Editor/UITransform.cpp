#include "UITransform.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Input/Input.hpp"

void UITransform::init()
{
    GameObject::init();
    mPosition = mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UIVector>();
    mPosition->mIsStatic = mIsStatic;
    mTransform->addChild(mPosition->mTransform);
    mPosition->mTransform->setLocalPosition(Maths::Vector3(0,-0.05 * 0,0));
    mRotation = mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UIVector>();
    mRotation->mIsStatic = mIsStatic;
    mTransform->addChild(mRotation->mTransform);
    mRotation->mTransform->setLocalPosition(Maths::Vector3(0,-0.05 * 1,0));
    mScale = mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UIVector>();
    mScale->mIsStatic = mIsStatic;
    mTransform->addChild(mScale->mTransform);
    mScale->mTransform->setLocalPosition(Maths::Vector3(0,-0.05 * 2,0));
}

void UITransform::update(const Transform* transform)
{
    mPosition->update(transform->getWorldPosition());
    mRotation->update(transform->getWorldRotation());
    mScale->update(transform->getWorldScale());
}

void UITransform::onDestroy()
{
    mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mPosition);
    mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mRotation);
    mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mScale);
    GameObject::onDestroy();
}