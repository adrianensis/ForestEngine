#include "UIVector.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Input/Input.hpp"

void UIVector::init()
{
    GameObject::init();

    // UIBuilder uiBuilder;
	// uiBuilder.
	// setPosition(Maths::Vector2(0,0)).
	// // setPosition(startGlyph).
	// setAdjustSizeToText(true).
	// setIsStatic(mIsStatic).
	// setIsStaticText(false).
	// setIsAffectedByLayout(true).
    // setParent(this).
	// setSize(Maths::Vector2(0.5, 0.05f));

    // mX = uiBuilder.
	// setText(Core::HashedString("000000.000")).
    // // setIsStatic(false).
	// create<UIText>(mScene->getInternalPointer()).
	// getUIElement<UIText>();
    // mY = uiBuilder.
	// setText(Core::HashedString("000000.000")).
    // // setIsStatic(false).
	// create<UIText>(mScene->getInternalPointer()).
	// getUIElement<UIText>();
    // mZ = uiBuilder.
	// setText(Core::HashedString("000000.000")).
    // // setIsStatic(false).
	// create<UIText>(mScene->getInternalPointer()).
	// getUIElement<UIText>();
    // // mW = uiBuilder.
	// // setText(Core::HashedString(std::to_string(v.x))).
    // // setIsStatic(false).
	// // create<UIText>().
	// // getUIElement<UIText>();
}

void UIVector::update(const Maths::Vector3& v)
{
    mX->setText(Core::HashedString(std::to_string(v.x).substr(0, 10)));
    mY->setText(Core::HashedString(std::to_string(v.y).substr(0, 10)));
    mZ->setText(Core::HashedString(std::to_string(v.z).substr(0, 10)));
}

void UIVector::onDestroy()
{
    mScene->removeGameObject(mX);
    mScene->removeGameObject(mY);
    mScene->removeGameObject(mZ);
    mScene->removeGameObject(mW);
    GameObject::onDestroy();
}