#include "UI/UIElements/UIList.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIElements/UIText.hpp"
#include "UI/UIElements/UIButton.hpp"

#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "GPU/GPUInstance.hpp"
#include "Window/WindowManager.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "GPU/Mesh/GPUMeshFactory.hpp"

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
	mTransform->setLocalScale(Maths::Vector3(UIUtils::correctAspectRatioVectorX(mConfig.mSize), 1));

    GPURenderItemData rendererData;
	rendererData.mMesh = GPUInstance::getInstance().mGPUMeshFactory->getPrimitive<Maths::Cube>();
	rendererData.mShader = mConfig.mShader;
	// rendererData.setColor(mConfig.mStyle->mBackgroundColor);
    rendererData.mGPUShaderStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        Core::ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.getId()
    };

	//renderer->setClipRectangle(Maths::Cube(Maths::Vector2(mConfig.mPosition.x, mConfig.mPosition.y), Maths::Vector2(mConfig.mSize.x / GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio(), mConfig.mSize.y)));
	
	EC::ComponentPtr<MeshRenderer> renderer = ECManager.requestComponent<MeshRenderer>();
	renderer->init(rendererData);
    ECManager.addComponent(ECManager.getEntityPtr(this), renderer);

	renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mDepth = mConfig.mLayer;
	renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = mConfig.mStyle->mBackgroundColor;
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();

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
	// NEXT : Temporary
	if (mButtons.empty())
	{
		Maths::Vector3 scale = mTransform->getLocalScale();
		scale.x = scale.x * GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio();

		UIBuilder uiBuilder;

		uiBuilder.
			setLayout(UILayout::VERTICAL).
			//setSize(scale).
			setPosition(Maths::Vector2((-scale.x / 2.0f) / GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio(), scale.y/2.0f)).
			setTextScale(mConfig.mTextScale).
			setAdjustSizeToText(true).
			setIsStatic(false).
			setLayer(mConfig.mLayer + 1).
            setParent(ECManager.getEntityPtr(this));

		FOR_LIST(it, mEntries)
		{
			Core::HashedString label = Core::HashedString((*it).mLabel);
			UIElementCallback onPressedCallback = (*it).mCallback;

			uiBuilder.
			setText(label).
			create<UIButton>();


			EC::EntityPtr<UIButton> button = uiBuilder.getUIElement<UIButton>();
			button->setOnPressedCallback(onPressedCallback);

			mButtons.push_back(button);
		}

		//setEntriesVisibility(false);
	}

	FOR_LIST(it, mButtons)
	{
		//(*it)->setVisibility(!(*it)->isVisible());
	}

	// NEXT : If I want to create-remove buttons, I have to implement TIMER NEXT FRAME!
	//setEntriesVisibility(mButtons->isEmpty());
}

void UIList::setEntriesVisibility(bool visible)
{
	/*if(visible){
		FOR_LIST(it, mEntries) {

			std::string& label = it->mLabel;
			UIElementCallback onPressedCallback = it->mCallback;

			Maths::Vector3 scale = mTransform->getLocalScale();
			scale.x = scale.x * GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio();

			GET_SYSTEM(UIManager).getBuilder()->saveData()->
				setLocalPosition(Maths::Vector2(-scale.x/2.0f,-scale.y* mButtons->getLength() - scale.y/2.0f))->
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
				mScene->removeSceneObject(it.get());
			}

			mButtons->clear();
		} 
	}*/

	FOR_LIST(it, mButtons)
	{
		(*it)->setVisibility(visible);
	}
}

void UIList::onScroll(Core::f32 scroll)
{
	UIElement::onScroll(scroll);

	if (isVisible())
	{
		if (isMouseCursorInsideElement())
		{
			FOR_LIST(it, mButtons)
			{
				(*it)->mTransform->addLocalTranslation(Maths::Vector2(0,0.005f * -scroll));
			}
		}
	}
}