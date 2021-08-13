#include "InfoBar.hpp"

#include "UI/UI.hpp"
#include "UI/UIDropdown.hpp"
#include "UI/UIList.hpp"

#include "EditorController.hpp"

void InfoBar::init(EditorController* editorController)
{
    mEditorController = editorController;

	UI::getInstance()->getUIBuilder().
	setLayout(UILayout::HORIZONTAL).
	setPosition(Vector2(-1,-0.95)).
	setLayer(0).
	setAdjustSizeToText(false).
	setTextSize(Vector2(0.025f, 0.05f)).
	setSize(Vector2(0.35f, 0.05f));

	mFPS = UI::getInstance()->getUIBuilder().
	setText("FPS 0").
	create<UIText>().
	getUIElement<UIText>();

	mLayer = UI::getInstance()->getUIBuilder().
	setText("Layer 0").
	create<UIText>().
	getUIElement<UIText>();
}

void InfoBar::setLayer(u32 layer)
{
	mLayer->setText("Layer " + std::to_string(layer));
}

void InfoBar::setFPS(f32 fps)
{
	mFPS->setText("FPS " + std::to_string(fps));
}