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

	mTransform.get().mLocalPosition = (mConfig.mDisplayPosition);
	mTransform.get().mScale = (Vector3(UIUtils::correctAspectRatio_X(mConfig.mSize), 1));
	mTransform.get().mAffectedByProjection = (false);

	OwnerPtr<Renderer> renderer = OwnerPtr<Renderer>::newObject();
	renderer.get().init();

	renderer.get().mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
	renderer.get().mMaterial = (mConfig.mMaterial);
	renderer.get().mColor = (mConfig.mStyle->mBackgroundColor);
	renderer.get().mDepth = (mConfig.mLayer);
    renderer.get().mIsStencilMask = (true);
    renderer.get().mStencilValue = (0x2);
	//renderer->setHasBorder(true);

	//renderer->setClipRectangle(Rectangle(Vector2(mConfig.mPosition.x, mConfig.mPosition.y), Vector2(mConfig.mSize.x / GET_SYSTEM(RenderContext).getAspectRatio(), mConfig.mSize.y)));
	
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
		Vector3 scale = mTransform.get().mScale;
		scale.x = scale.x * GET_SYSTEM(RenderContext).getAspectRatio();

		UIBuilder uiBuilder;

		uiBuilder.
			setLayout(UILayout::VERTICAL).
			//setSize(scale).
			setPosition(Vector2((-scale.x / 2.0f) / GET_SYSTEM(RenderContext).getAspectRatio(), scale.y/2.0f)).
			setTextSize(mConfig.mTextSize).
			setAdjustSizeToText(true).
			setLayer(mConfig.mLayer).
            setStencilValue(0x02).
            setStencilFunction(GL_EQUAL).
            setParent(Ptr<GameObject>::cast(getPtrToThis()));

		FOR_LIST(it, mEntries)
		{
			std::string& label = (*it).mLabel;
			UIElementCallback onPressedCallback = (*it).mCallback;

			uiBuilder.
			setText(label).
			create<UIButton>();


			Ptr<UIButton> button = uiBuilder.getUIElement<UIButton>();
			button.get().setOnPressedCallback(onPressedCallback);
            //button->getRenderer()->mStencilValue = (0x2);
			//button->setVisibility(false);

			// Transform *t = &button->mTransform.get();
			// t->setParent(&mTransform.get());

			// Rectangle clipRectangle(
			// 			Vector2(mConfig.mPosition.x, mConfig.mPosition.y),
			// 			Vector2(mConfig.mSize.x / GET_SYSTEM(RenderContext).getAspectRatio(), mConfig.mSize.y));

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

			Vector3 scale = mTransform.get().getScale();
			scale.x = scale.x * GET_SYSTEM(RenderContext).getAspectRatio();

			GET_SYSTEM(UIManager).getBuilder()->saveData()->
				setPosition(Vector2(-scale.x/2.0f,-scale.y* mButtons->getLength() - scale.y/2.0f))->
				setSize(scale)->
				setText(label)->
				setAdjustSizeToText(true)->
				setLayer(getRenderer()->getLayer() + 1)->
				setIsAffectedByLayout(false)->
				create<UIButton>();

			UIButton* button = (UIButton*) GET_SYSTEM(UIManager).getBuilder()->getUIElement();
			button->setOnPressedCallback(onPressedCallback);
			//button->setVisibility(false);

			Transform* t = button->mTransform;
			t->setParent(mTransform);

			GET_SYSTEM(UIManager).getBuilder()->restoreData();

			mButtons->pushBack(button);
		}
	}
	else {
		if(!mButtons->isEmpty()){
			FOR_LIST(it, mButtons){
				mScene.get().removeGameObject(it.get());
			}

			mButtons->clear();
		} 
	}*/

	FOR_LIST(it, mButtons)
	{
		(*it).get().setVisibility(visible);
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
				(*it).get().mTransform.get().translate(Vector2(0,0.005f * -scroll));
			}
		}
	}
}