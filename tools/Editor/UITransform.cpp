#include "UITransform.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"

void UITransform::init()
{
    SceneObject::init();
    mPosition = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UIVector>();
    mPosition->mIsStatic = mIsStatic;
    mTransform->addChild(mPosition->mTransform);
    mPosition->mTransform->setLocalPosition(Maths::Vector3(0,-0.05 * 0,0));
    mRotation = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UIVector>();
    mRotation->mIsStatic = mIsStatic;
    mTransform->addChild(mRotation->mTransform);
    mRotation->mTransform->setLocalPosition(Maths::Vector3(0,-0.05 * 1,0));
    mScale = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UIVector>();
    mScale->mIsStatic = mIsStatic;
    mTransform->addChild(mScale->mTransform);
    mScale->mTransform->setLocalPosition(Maths::Vector3(0,-0.05 * 2,0));
}

void UITransform::update(EC::ComponentPtr<const Transform> transform)
{
    mPosition->update(transform->getWorldPosition());
    mRotation->update(transform->getWorldRotation());
    mScale->update(transform->getWorldScale());
}

void UITransform::onDestroy()
{
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mPosition);
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mRotation);
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mScale);
    SceneObject::onDestroy();
}