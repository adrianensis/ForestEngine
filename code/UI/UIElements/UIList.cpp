#include "UI/UIElements/UIList.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIElements/UIText.hpp"
#include "UI/UIElements/UIButton.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void UIListEntry::init(const std::string& label, UIElementCallback callback)
{
	mLabel = label;
	mCallback = callback;
}

void UIList::init()
{
	UIElement::init();

    mClipChildren = true;

	setOnFocusLostCallback([this](UIElement *uiElement)
	{ 
		//setEntriesVisibility(false); 
	});

	subscribeToScrollEvents();
}

void UIList::initFromConfig(const UIElementConfig& config)
{
	UIElement::initFromConfig(config);

	mTransform->setLocalPosition(mConfig.mDisplayPosition);
	mTransform->setLocalScale(Vector3(UIUtils::correctAspectRatio_X(mConfig.mSize), 1));

    RendererData rendererData;
	rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
	rendererData.mMaterial = mConfig.mMaterial;
    rendererData.mIsInstanced = true;
	// rendererData.setColor(mConfig.mStyle->mBackgroundColor);
    rendererData.mStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.getId()
    };

	//renderer->setClipRectangle(Rectangle(Vector2(mConfig.mPosition.x, mConfig.mPosition.y), Vector2(mConfig.mSize.x / GET_SYSTEM(Window).getAspectRatio(), mConfig.mSize.y)));
	
    Ptr<MeshRenderer> renderer = createComponent<MeshRenderer>(rendererData);
	renderer->getMaterialInstance().mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mDepth = mConfig.mLayer;
	renderer->getMaterialInstance().mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mColor = mConfig.mStyle->mBackgroundColor;

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
		Vector3 scale = mTransform->getLocalScale();
		scale.x = scale.x * GET_SYSTEM(Window).getAspectRatio();

		UIBuilder uiBuilder;

		uiBuilder.
			setLayout(UILayout::VERTICAL).
			//setSize(scale).
			setPosition(Vector2((-scale.x / 2.0f) / GET_SYSTEM(Window).getAspectRatio(), scale.y/2.0f)).
			// setTextSize(mConfig.mTextSize).
			setAdjustSizeToText(true).
			setIsStatic(false).
			setLayer(mConfig.mLayer + 1).
            setParent(Ptr<GameObject>::cast(getPtrToThis<UIList>()));

		FOR_LIST(it, mEntries)
		{
			std::string& label = (*it).mLabel;
			UIElementCallback onPressedCallback = (*it).mCallback;

			uiBuilder.
			setText(label).
			create<UIButton>();


			Ptr<UIButton> button = uiBuilder.getUIElement<UIButton>();
			button->setOnPressedCallback(onPressedCallback);

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

			std::string& label = it->mLabel;
			UIElementCallback onPressedCallback = it->mCallback;

			Vector3 scale = mTransform->getLocalScale();
			scale.x = scale.x * GET_SYSTEM(Window).getAspectRatio();

			GET_SYSTEM(UIManager).getBuilder()->saveData()->
				setLocalPosition(Vector2(-scale.x/2.0f,-scale.y* mButtons->getLength() - scale.y/2.0f))->
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
				mScene->removeGameObject(it.get());
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
				(*it)->mTransform->addLocalTranslation(Vector2(0,0.005f * -scroll));
			}
		}
	}
}