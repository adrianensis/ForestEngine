#include "UI/UIElements/UIList.hpp"
#include "Graphics/Mesh/MeshFactory.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIElements/UIText.hpp"
#include "UI/UIElements/UIButton.hpp"

#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Window/WindowManager.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "Graphics/Mesh/MeshFactory.hpp"

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

void UIList::initFromConfig(UIManager* uiManager, const UIElementConfig& config)
{
	UIElement::initFromConfig(uiManager, config);

	mTransform->setLocalPosition(mConfig.mDisplayPosition);
	mTransform->setLocalScale(Maths::Vector3(UIUtils::correctAspectRatioVectorX(GET_SYSTEM(Window::WindowManager).getMainWindow().getInternalPointer(), mConfig.mSize), 1));

    GPURenderItemData rendererData;
	rendererData.mMesh = MeshFactory::getInstance().getPrimitive<Maths::Rectangle>();
	rendererData.mShader = mConfig.mShader;
	// rendererData.setColor(mConfig.mStyle->mBackgroundColor);
    rendererData.mGPUDepthStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        Core::ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.getId()
    };

	//renderer->setClipRectangle(Maths::Cube(Maths::Vector2(mConfig.mPosition.x, mConfig.mPosition.y), Maths::Vector2(mConfig.mSize.x / (Window::WindowManager).getMainWindow()->getAspectRatio(), mConfig.mSize.y)));
	
	MeshRenderer* renderer = GET_SYSTEM(EC::EntityComponentManager).requestComponent<MeshRenderer>(this, [&](auto* component)
		{
			component->init(rendererData);
		});

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

		UIBuilder uiBuilder = mUIManager->createUIBuilder();

		uiBuilder.
			setLayout(UILayout::VERTICAL).
			//setSize(scale).
			setPosition(Maths::Vector2((-scale.x / 2.0f) / GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio(), scale.y/2.0f)).
			setTextScale(mConfig.mTextScale).
			setAdjustSizeToText(true).
			setIsStatic(false).
			setLayer(mConfig.mLayer + 1).
            setParent(this);

		FOR_LIST(it, mEntries)
		{
			Core::HashedString label = Core::HashedString((*it).mLabel);
			UIElementCallback onPressedCallback = (*it).mCallback;

			uiBuilder.
			setText(label).
			create<UIButton>(mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer());


			UIButton* button = uiBuilder.getUIElement<UIButton>();
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
			scale.x = scale.x * (Window::WindowManager).getMainWindow()->getAspectRatio();

			mUIManager->getBuilder()->saveData()->
				setLocalPosition(Maths::Vector2(-scale.x/2.0f,-scale.y* mButtons->getLength() - scale.y/2.0f))->
				setSize(scale)->
				setText(label)->
				setAdjustSizeToText(true)->
				setLayer(getRenderer()->getLayer() + 1)->
				setIsAffectedByLayout(false)->
				create<UIButton>();

			UIButton* button = (UIButton*) mUIManager->getBuilder()->getUIElement();
			button->setOnPressedCallback(onPressedCallback);
			//button->setVisibility(false);

			Transform* t = button->mTransform;
			t->setParent(mTransform);

			mUIManager->getBuilder()->restoreData();

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