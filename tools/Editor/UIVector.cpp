#include "UIVector.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"

void UIVector::init()
{
    SceneObject::init();

    UIBuilder uiBuilder;
	uiBuilder.
	setPosition(Vector2(0,0)).
	// setPosition(startGlyph).
	setAdjustSizeToText(true).
	setIsStatic(mIsStatic).
	setIsStaticText(false).
	setIsAffectedByLayout(true).
    setParent(this).
	setTextScale(0.5f).
	setSize(Vector2(0.5, 0.05f));

    mX = uiBuilder.
	setText(Core::HashedString("000000.000")).
    // setIsStatic(false).
	create<UIText>().
	getUIElement<UIText>();
    mY = uiBuilder.
	setText(Core::HashedString("000000.000")).
    // setIsStatic(false).
	create<UIText>().
	getUIElement<UIText>();
    mZ = uiBuilder.
	setText(Core::HashedString("000000.000")).
    // setIsStatic(false).
	create<UIText>().
	getUIElement<UIText>();
    // mW = uiBuilder.
	// setText(Core::HashedString(std::to_string(v.x))).
    // setIsStatic(false).
	// create<UIText>().
	// getUIElement<UIText>();
}

void UIVector::update(const Vector3& v)
{
    mX->setText(Core::HashedString(std::to_string(v.x).substr(0, 10)));
    mY->setText(Core::HashedString(std::to_string(v.y).substr(0, 10)));
    mZ->setText(Core::HashedString(std::to_string(v.z).substr(0, 10)));
}

void UIVector::onDestroy()
{
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mX);
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mY);
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mZ);
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mW);
    SceneObject::onDestroy();
}