#include "UITransform.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"

void UITransform::init()
{
    GameObject::init();
    mPosition = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<UIVector>();
    mTransform->addChild(mPosition->mTransform);
    mPosition->mTransform->setLocalPosition(Vector3(0,-0.05 * 0,0));
    mRotation = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<UIVector>();
    mTransform->addChild(mRotation->mTransform);
    mRotation->mTransform->setLocalPosition(Vector3(0,-0.05 * 1,0));
    mScale = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<UIVector>();
    mTransform->addChild(mScale->mTransform);
    mScale->mTransform->setLocalPosition(Vector3(0,-0.05 * 2,0));
}

void UITransform::update(Ptr<const Transform> transform)
{
    mPosition->update(transform->getWorldPosition());
    mRotation->update(transform->getWorldRotation());
    mScale->update(transform->getWorldScale());
}

void UITransform::onDestroy()
{
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mPosition);
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mRotation);
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mScale);
    GameObject::onDestroy();
}