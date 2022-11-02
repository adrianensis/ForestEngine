#include "UI/UIElements/UIList.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIElements/UIText.hpp"
#include "UI/UIElements/UIButton.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"

void UIListEntry::init(const std::string& label, UIElementCallback callback)
{
	mLabel = label;
	mCallback = callback;
}

void UIList::init()
{
	UIElement::init();

	setOnFocusLostCallback([this](UIElement *uiElement)
	{ 
		//setEntriesVisibility(false); 
	});

	subscribeToScrollEvents();
}

void UIList::initFromConfig(const UIElementConfig& config)
{
	UIElement::initFromConfig(config);

	getTransform().get().setLocalPosition(mConfig.mDisplayPosition);
	getTransform().get().setScale(Vector3(UIUtils::correctAspectRatio_X(mConfig.mSize), 1));
	getTransform().get().setAffectedByProjection(false);

	Renderer* renderer = NEW(Renderer);
	renderer->init();

	renderer->setMesh(MeshPrimitives::getInstance().getPrimitive<Rectangle>());
	renderer->setMaterial(mConfig.mMaterial);
	renderer->setColor(mConfig.mStyle->mBackgroundColor);
	renderer->setDepth(mConfig.mLayer);
	//renderer->setHasBorder(true);

	//renderer->setClipRectangle(Rectangle(Vector2(mConfig.mPosition.x, mConfig.mPosition.y), Vector2(mConfig.mSize.x / RenderContext::getAspectRatio(), mConfig.mSize.y)));
	
	addComponent<Renderer>(renderer);
	
	setComponentsCache();

	setOnPressedCallback([&](UIElement* uiElement) 
	{
		toggle();
	});

	toggle();
}

void UIList::onDestroy()
{
	UIElement::onDestroy();
}

UIList& UIList::addOption(const std::string& label, UIElementCallback onPressedCallback)
{
	UIListEntry entry;
	entry.init(label, onPressedCallback);
	mEntries.push_back(entry);
	return *this;
}

void UIList::toggle()
{
	// TODO : Temporary
	if (mButtons.empty())
	{
		Vector3 scale = getTransform().get().getScale();
		scale.x = scale.x * RenderContext::getAspectRatio();

		UIBuilder uiBuilder;

		uiBuilder.
			setLayout(UILayout::VERTICAL).
			//setSize(scale).
			setPosition(Vector2((-scale.x / 2.0f) / RenderContext::getAspectRatio(), scale.y/2.0f)).
			setTextSize(mConfig.mTextSize).
			setAdjustSizeToText(true).
			setLayer(mConfig.mLayer).
            setParent(this);

		FOR_LIST(it, mEntries)
		{
			std::string& label = (*it).mLabel;
			UIElementCallback onPressedCallback = (*it).mCallback;

			uiBuilder.
			setText(label).
			create<UIButton>();

			UIButton *button = (UIButton *)uiBuilder.getUIElement();
			button->setOnPressedCallback(onPressedCallback);
			//button->setVisibility(false);

			// Transform *t = &button->getTransform().get();
			// t->setParent(&getTransform().get());

			// Rectangle clipRectangle(
			// 			Vector2(mConfig.mPosition.x, mConfig.mPosition.y),
			// 			Vector2(mConfig.mSize.x / RenderContext::getAspectRatio(), mConfig.mSize.y));

			// button->getRenderer()->setClipRectangle(clipRectangle);

			// Set clip rectangle for UIText label also
			// FOR_LIST(itRenderer, button->getText()->getComponents<Renderer>())
			// {
			// 	(*itRenderer).get().setClipRectangle(clipRectangle);
			// }

			mButtons.push_back(button);
		}

		//setEntriesVisibility(false);
	}

	FOR_LIST(it, mButtons)
	{
		//(*it)->setVisibility(!(*it)->isVisible());
	}

	// TODO : If I want to create-remove buttons, I have to implement TIMER NEXT FRAME!
	//setEntriesVisibility(mButtons->isEmpty());
}

void UIList::setEntriesVisibility(bool visible)
{
	/*if(visible){
		FOR_LIST(it, mEntries) {

			std::string& label = it.get().mLabel;
			UIElementCallback onPressedCallback = it.get().mCallback;

			Vector3 scale = getTransform().get().getScale();
			scale.x = scale.x * RenderContext::getAspectRatio();

			UIManager::getInstance().getBuilder()->saveData()->
				setPosition(Vector2(-scale.x/2.0f,-scale.y* mButtons->getLength() - scale.y/2.0f))->
				setSize(scale)->
				setText(label)->
				setAdjustSizeToText(true)->
				setLayer(getRenderer()->getLayer() + 1)->
				setIsAffectedByLayout(false)->
				create<UIButton>();

			UIButton* button = (UIButton*) UIManager::getInstance().getBuilder()->getUIElement();
			button->setOnPressedCallback(onPressedCallback);
			//button->setVisibility(false);

			Transform* t = button->getTransform();
			t->setParent(getTransform());

			UIManager::getInstance().getBuilder()->restoreData();

			mButtons->pushBack(button);
		}
	}
	else {
		if(!mButtons->isEmpty()){
			FOR_LIST(it, mButtons){
				getScene()->removeGameObject(it.get());
			}

			mButtons->clear();
		} 
	}*/

	FOR_LIST(it, mButtons)
	{
		(*it)->setVisibility(visible);
	}
}

void UIList::onScroll(f32 scroll)
{
	UIElement::onScroll(scroll);

	if (isVisible())
	{
		if (isMouseCursorInsideElement())
		{
			FOR_LIST(it, mButtons)
			{
				(*it)->getTransform().get().translate(Vector2(0,0.005f * -scroll));
			}
		}
	}
}