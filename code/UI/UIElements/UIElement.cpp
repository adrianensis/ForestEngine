#include "UI/UIElements/UIElement.hpp"

#include "Engine/EntityComponent/Entity.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Window/Window.hpp"

#include "UI/UIManager.hpp"
#include "UI/UIGroup.hpp"

void UIElement::initFromConfig(const UIElementConfig& config)
{
	mConfig = config;
    mIsStatic = mConfig.mIsStatic;

    mConfig = calculateConfig(mConfig);
}

void UIElement::onDestroy()
{
	SceneObject::onDestroy();
	UNSUBSCRIBE_TO_EVENT(Input::InputEventKeyPressed, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(Input::InputEventKeyReleased, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(Input::InputEventMouseButtonPressed, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(Input::InputEventMouseButtonReleased, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(Input::InputEventScroll, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(Input::InputEventChar, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(Input::InputEventKeyBackspace, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(Input::InputEventKeyEnter, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(Input::InputEventKeyEsc, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(Input::InputEventMouseMoved, nullptr, this);

	if (hasFocus())
	{
		GET_SYSTEM(UIManager).setFocusedElement(EC::TEntityPtr<UIElement>());
	}
}

bool UIElement::hasFocus() const
{
	return GET_SYSTEM(UIManager).getFocusedElement() == this;
}

bool UIElement::isMouseCursorInsideElement() const
{
	PROFILER_CPU()

    bool parentCheck = true;
    if(mConfig.mParent)
    {
        if(EC::TEntityPtr<UIElement> parentUIElement = mConfig.mParent)
        {
            parentCheck = parentUIElement->isMouseCursorInsideElement();
        }
    }

    if(!parentCheck)
    {
        return false;
    }

	Maths::Vector2 mousePosition = GET_SYSTEM(Input::Input).getMousePosition();

	// if(mTransform->mGeometricSpace == Maths::GeometricSpace::WORLD)
	{
		// mousePosition = GET_SYSTEM(ScenesManager).getCurrentCamera()->screenToWorld(GET_SYSTEM(Input::Input).getMousePosition());
	}

    // GET_SYSTEM(DebugRenderer).drawRectangle(Maths::Cube(getLeftTopPosition(), correctedSize), 1, false);

	return Maths::Geometry::testCubePoint(
		Maths::Cube(getLeftTopPosition(), mConfig.mDisplaySize),
		mousePosition, 0);
}

Maths::Vector3 UIElement::getLeftTopPosition() const
{
    PROFILER_CPU()
    Maths::Vector3 position = mTransform->getWorldPosition();
    position.x = position.x - (mConfig.mDisplaySize.x / 2.0f);
    position.y = position.y + (mConfig.mDisplaySize.y / 2.0f);

    return position;
}

void UIElement::setVisibility(bool visibility)
{
    releaseFocus();
	ECManager.setEntityActive(ECManager.getEntityPtr(this), visibility);
}

bool UIElement::isVisible()
{
	return isActive();
}

void UIElement::setOnPressedCallback(UIElementCallback callback)
{
	mOnPressedFunctor.mUIElement = this;
	mOnPressedFunctor.mCallback = callback;
}

void UIElement::setOnTextChangedCallback(UIElementCallback callback)
{
	mOnTextChangedFunctor.mUIElement = this;
	mOnTextChangedFunctor.mCallback = callback;
}

void UIElement::setOnFocusLostCallback(UIElementCallback callback)
{
	mOnFocusLostFunctor.mUIElement = this;
	mOnFocusLostFunctor.mCallback = callback;
}

void UIElement::postInit()
{
    mRenderer = ECManager.getFirstComponent<MeshRenderer>(ECManager.getEntityPtr(this));
}

void UIElement::subscribeToKeyEvents()
{
	SUBSCRIBE_TO_EVENT(Input::InputEventKeyPressed, nullptr, this, [this](const Event::Event *event)
	{
    	PROFILER_CPU()
		if (!isVisible()) { return; }
	});

	SUBSCRIBE_TO_EVENT(Input::InputEventKeyReleased, nullptr, this, [this](const Event::Event *event)
	{
    	PROFILER_CPU()
		if (!isVisible()) { return; }
	});
}

void UIElement::subscribeToCharEvents()
{
	SUBSCRIBE_TO_EVENT(Input::InputEventChar, nullptr, this, [this](const Event::Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        onCharEventReceived(((const Input::InputEventChar *)event)->mChar);
	});

	SUBSCRIBE_TO_EVENT(Input::InputEventKeyBackspace, nullptr, this, [this](const Event::Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        onBackspaceEventReceived();
	});
}

void UIElement::subscribeToMouseEvents()
{
	SUBSCRIBE_TO_EVENT(Input::InputEventMouseButtonPressed, nullptr, this, [this](const Event::Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        const Input::InputEventMouseButtonPressed *e = (const Input::InputEventMouseButtonPressed *)event;

        if (e->mButton == GLFW_MOUSE_BUTTON_LEFT)
        {
            onPressedEventReceived();
        }
	});

	SUBSCRIBE_TO_EVENT(Input::InputEventMouseButtonReleased, nullptr, this, [this](const Event::Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        const Input::InputEventMouseButtonReleased *e = (const Input::InputEventMouseButtonReleased *)event;
        if (e->mButton == GLFW_MOUSE_BUTTON_LEFT)
        {
            onReleasedEventReceived();
        }
	});

	SUBSCRIBE_TO_EVENT(Input::InputEventMouseMoved, nullptr, this, [this](const Event::Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        onMouseMovedEventReceived();
	});
}

void UIElement::subscribeToScrollEvents()
{
	SUBSCRIBE_TO_EVENT(Input::InputEventScroll, nullptr, this, [this](const Event::Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        const Input::InputEventScroll *e = (const Input::InputEventScroll *)event;
        onScrollEventReceived(e->mScroll);
	});
}

void UIElement::subscribeToEnterEvent()
{
	SUBSCRIBE_TO_EVENT(Input::InputEventKeyEnter, nullptr, this, [this](const Event::Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        onEnterEventReceived();
	});
}

void UIElement::subscribeToEscEvent()
{
	SUBSCRIBE_TO_EVENT(Input::InputEventKeyEsc, nullptr, this, [this](const Event::Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        onEscEventReceived();
	});
}

void UIElement::onPressedEventReceived()
{
	bool cursorInside = isMouseCursorInsideElement();
    if (!cursorInside) { return; }

    mState = UIElementState::PRESSED;
    setColorPressed();
    requestFocus();

    if (mConsumeInput)
    {
        GET_SYSTEM(Input::Input).clearMouseButton();
    }
}

void UIElement::onReleasedEventReceived()
{
	if(mState != UIElementState::PRESSED || !hasFocus()) { return; }

    bool cursorInside = isMouseCursorInsideElement();
    bool shouldRelease = mOnlyReleaseOnClickOutside ? !cursorInside : cursorInside;
    if(!shouldRelease) { return; }

    if (mConsumeInput)
    {
        GET_SYSTEM(Input::Input).clearMouseButton();
    }

    onPrePressed();
    
    mOnPressedFunctor.execute();
    mState = UIElementState::RELEASED;
    setColorRelease();
    releaseFocus();

    onPostPressed();
}

void UIElement::onMouseMovedEventReceived()
{
    PROFILER_CPU()
	if(mState != UIElementState::RELEASED) { return; }

    setColorHover();

    if (isMouseCursorInsideElement())
    {
        onMouseOverEnter();
    }
    else
    {
        onMouseOverExit();
    }
}

void UIElement::onScrollEventReceived(Core::f32 scrollValue)
{
	if (!isMouseCursorInsideElement()) { return; }
    
    scroll(scrollValue);
}

void UIElement::onCharEventReceived(char character)
{
	if (!hasFocus()) { return; }

    mInputString.push_back(character);
    setText(Core::HashedString(mInputString));
    onChar(character);
}

void UIElement::onBackspaceEventReceived()
{
	if(mInputString.empty()) { return; }

    mInputString.pop_back();
    setText(Core::HashedString(mInputString));
    onBackspace();
}

void UIElement::onEnterEventReceived()
{
	if (!hasFocus()) { return; }

    onEnter();
    releaseFocus();
}

void UIElement::onEscEventReceived()
{
	if (!hasFocus()) { return; }

    onEsc();
    releaseFocus();
}

void UIElement::releaseFocus()
{
    if (!hasFocus()) { return; }

    GET_SYSTEM(UIManager).setFocusedElement(EC::TEntityPtr<UIElement>());
    mOnFocusLostFunctor.execute();
    onFocusLost();
}

void UIElement::requestFocus()
{
    if (hasFocus()) { return; }

    EC::TEntityPtr<UIElement> lastFocusedElement = GET_SYSTEM(UIManager).getFocusedElement();
    if (lastFocusedElement)
    {
        lastFocusedElement->releaseFocus();
    }

    GET_SYSTEM(UIManager).setFocusedElement(ECManager.getEntityPtr(this));

    mInputString.clear();
    setText(Core::HashedString(mInputString));
    onFocus();
}

void UIElement::scroll(Core::f32 scrollValue)
{
	mOnScrollFunctor.execute();
	onScroll(scrollValue);
}

// void UIElement::releaseOtherToggleElements()
// {
// 	const UIGroup& group = GET_SYSTEM(UIManager).getOrCreateGroup(mConfig.mGroup);
// 	FOR_LIST(it, group.getUIElements())
// 	{
// 		EC::TEntityPtr<UIElement> other = *it;
// 		if(other != getPtrToThis<UIElement>())
// 		{
// 			if(other->getConfig().mToggleEnabled and
// 			other->getState() == UIElementState::TOGGLED and
// 			other->getConfig().mReleaseOnSameGroupPressed and
// 			!other->getConfig().mGroup.empty() and
// 			other->getConfig().mGroup == mConfig.mGroup)
// 			{
// 				other->executeRelease();
// 			}
// 		}
// 	}
// }

void UIElement::setColorPressed()
{
    if(!mRenderer) { return; }

    mRenderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = mConfig.mStyle->mColorPressed;
    mRenderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
}

void UIElement::setColorRelease()
{
    if(!mRenderer) { return; }

    bool cursorInside = isMouseCursorInsideElement();
    mRenderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = cursorInside ? mConfig.mStyle->mColorHovered : mConfig.mStyle->mBackgroundColor;
    mRenderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
}

void UIElement::setColorHover()
{
    if(!mRenderer) { return; }

    bool cursorInside = isMouseCursorInsideElement();
    mRenderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = cursorInside ? mConfig.mStyle->mColorHovered : mConfig.mStyle->mBackgroundColor;
    mRenderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
}

GPUShaderStencilData UIElement::calculateStencilData() const
{
    GPUShaderStencilData shaderStencilData;
    if(mClipChildren)
    {
        shaderStencilData.mUseStencil = true;
        shaderStencilData.mStencilValue = 0;
        shaderStencilData.mStencilFunction = GPUStencilFunction::EQUAL;
        shaderStencilData.mStencilPassOp = GPUStencilOp::INCR;

        if(mConfig.mParent)
        {
            EC::TEntityPtr<UIElement> parentUIElement = mConfig.mParent;
            if(parentUIElement)
            {
                GPUShaderStencilData parentStencilData = parentUIElement->calculateStencilData();
                if(parentStencilData.mUseStencil)
                {
                    shaderStencilData = parentStencilData;
                    // if direct parent is clipping shape, then increment mask
                    if(parentUIElement->mClipChildren)
                    {
                        shaderStencilData.mStencilValue++;
                        shaderStencilData.mParentId = parentStencilData.mId;
                    }
                }
            }
        }
    }
    else
    {
        if(mConfig.mParent)
        {
            EC::TEntityPtr<UIElement> parentUIElement = mConfig.mParent;
            if(parentUIElement)
            {
                GPUShaderStencilData parentStencilData = parentUIElement->calculateStencilData();
                if(parentStencilData.mUseStencil)
                {
                    shaderStencilData = parentStencilData;
                    // if direct parent is clipping shape, then increment mask
                    if(parentUIElement->mClipChildren)
                    {
                        shaderStencilData.mStencilValue++;
                        shaderStencilData.mParentId = parentStencilData.mId;
                    }
                
                    shaderStencilData.mStencilPassOp = GPUStencilOp::KEEP;
                }
            }
        }
    }

    shaderStencilData.mId = this->getEntityId();

    return shaderStencilData;
}