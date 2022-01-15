#include "LayersBar.hpp"

#include "EditorStyles.hpp"

#include "Graphics/Module.hpp"

#include "../EditorController.hpp"

#include "EditorUIGroups.hpp"

void LayersBar::init(EditorController* editorController)
{
	EditorUIElement::init(editorController);

	UIBuilder uiBuilder;

	uiBuilder.
	setLayout(UILayout::VERTICAL).
	setPosition(Vector2(0.94,0.8)).
	setGroup(EditorUIGroups::smLayersBar).
	setStyle(&UIStyleManager::getInstance().getOrAddStyle<UIStyleEditorToolsBar>()).
	setSize(Vector2(0.07f, 0.07f)).
	//setAdjustSizeToText(true).
	setMaterial(MaterialManager::getInstance().loadMaterial("resources/editor-icons/Layer.png"));

	FOR_RANGE(i, 0, 10)
	{
		i32 layer = i;
		std::string layerStr = std::to_string(layer);

		uiBuilder.
		setText(layerStr).
		create<UIToggleButton>().
		getUIElement<UIToggleButton>()->
		setOnPressedCallback([&, layer](UIElement* uiElement){
			getEditorController()->setLayer(layer);
			getEditorController()->getInfoBar().setLayer(getEditorController()->getLayer());
		});

		if(i == 0)
		{
			uiBuilder.
			getUIElement<UIToggleButton>()->simulateClick();
		}
	}
}

void LayersBar::setVisibility(bool visible)
{
	EditorUIElement::setVisibility(visible);
	UIManager::getInstance().getOrCreateGroup(EditorUIGroups::smLayersBar).setVisibility(getIsVisible());
}