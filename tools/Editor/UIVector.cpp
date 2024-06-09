#include "UIVector.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"

void UIVector::init()
{
    GameObject::init();

    UIBuilder uiBuilder;
	uiBuilder.
	setPosition(Vector2(0,0)).
	// setPosition(startGlyph).
	setAdjustSizeToText(true).
	setIsAffectedByLayout(true).
    setParent(getPtrToThis<GameObject>()).
	setSize(Vector2(0.5/2.0, 0.05f/2.0));

    mX = uiBuilder.
	setText(HashedString("000000.000")).
    // setIsStatic(false).
	create<UIText>().
	getUIElement<UIText>();
    mY = uiBuilder.
	setText(HashedString("000000.000")).
    // setIsStatic(false).
	create<UIText>().
	getUIElement<UIText>();
    mZ = uiBuilder.
	setText(HashedString("000000.000")).
    // setIsStatic(false).
	create<UIText>().
	getUIElement<UIText>();
    // mW = uiBuilder.
	// setText(HashedString(std::to_string(v.x))).
    // setIsStatic(false).
	// create<UIText>().
	// getUIElement<UIText>();
}

void UIVector::update(const Vector3& v)
{
    mX->setText(HashedString(std::to_string(v.x).substr(0, 10)));
    mY->setText(HashedString(std::to_string(v.y).substr(0, 10)));
    mZ->setText(HashedString(std::to_string(v.z).substr(0, 10)));
}

void UIVector::onDestroy()
{
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mX);
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mY);
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mZ);
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mW);
    GameObject::onDestroy();
}