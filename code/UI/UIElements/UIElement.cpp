#include "UI/UIElements/UIElement.hpp"

#include "Graphics/Module.hpp"

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
	UNSUBSCRIBE_TO_EVENT(InputEventKeyPressed, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(InputEventKeyReleased, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(InputEventMouseButtonPressed, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(InputEventMouseButtonReleased, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(InputEventScroll, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(InputEventChar, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(InputEventKeyBackspace, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(InputEventKeyEnter, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(InputEventKeyEsc, nullptr, this);
	UNSUBSCRIBE_TO_EVENT(InputEventMouseMoved, nullptr, this);

	if (hasFocus())
	{
		GET_SYSTEM(UIManager).setFocusedElement(TEntityHandler<UIElement>());
	}
}

bool UIElement::hasFocus() const
{
	return EntityHandler::getEntityHandler(*this) == GET_SYSTEM(UIManager).getFocusedElement();
}

bool UIElement::isMouseCursorInsideElement() const
{
	PROFILER_CPU()

    bool parentCheck = true;
    if(mConfig.mParent)
    {
        if(TEntityHandler<UIElement> parentUIElement = mConfig.mParent)
        {
            parentCheck = parentUIElement->isMouseCursorInsideElement();
        }
    }

    if(!parentCheck)
    {
        return false;
    }

	Vector2 mousePosition = GET_SYSTEM(Input).getMousePosition();

	// if(mTransform->mGeometricSpace == GeometricSpace::WORLD)
	{
		// mousePosition = GET_SYSTEM(ScenesManager).getCurrentCamera()->screenToWorld(GET_SYSTEM(Input).getMousePosition());
	}

    // GET_SYSTEM(DebugRenderer).drawRectangle(Rectangle(getLeftTopPosition(), correctedSize), 1, false);

	return Geometry::testCubePoint(
		Rectangle(getLeftTopPosition(), mConfig.mDisplaySize),
		mousePosition, 0);
}

Vector3 UIElement::getLeftTopPosition() const
{
    PROFILER_CPU()
    Vector3 position = mTransform->getWorldPosition();
    position.x = position.x - (mConfig.mDisplaySize.x / 2.0f);
    position.y = position.y + (mConfig.mDisplaySize.y / 2.0f);

    return position;
}

void UIElement::setVisibility(bool visibility)
{
    releaseFocus();
	setIsActive(visibility);
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
	mRenderer = getFirstComponent<MeshRenderer>();
}

void UIElement::subscribeToKeyEvents()
{
	SUBSCRIBE_TO_EVENT(InputEventKeyPressed, nullptr, this, [this](const Event *event)
	{
    	PROFILER_CPU()
		if (!isVisible()) { return; }
	});

	SUBSCRIBE_TO_EVENT(InputEventKeyReleased, nullptr, this, [this](const Event *event)
	{
    	PROFILER_CPU()
		if (!isVisible()) { return; }
	});
}

void UIElement::subscribeToCharEvents()
{
	SUBSCRIBE_TO_EVENT(InputEventChar, nullptr, this, [this](const Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        onCharEventReceived(((const InputEventChar *)event)->mChar);
	});

	SUBSCRIBE_TO_EVENT(InputEventKeyBackspace, nullptr, this, [this](const Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        onBackspaceEventReceived();
	});
}

void UIElement::subscribeToMouseEvents()
{
	SUBSCRIBE_TO_EVENT(InputEventMouseButtonPressed, nullptr, this, [this](const Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        const InputEventMouseButtonPressed *e = (const InputEventMouseButtonPressed *)event;

        if (e->mButton == GLFW_MOUSE_BUTTON_LEFT)
        {
            onPressedEventReceived();
        }
	});

	SUBSCRIBE_TO_EVENT(InputEventMouseButtonReleased, nullptr, this, [this](const Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        const InputEventMouseButtonReleased *e = (const InputEventMouseButtonReleased *)event;
        if (e->mButton == GLFW_MOUSE_BUTTON_LEFT)
        {
            onReleasedEventReceived();
        }
	});

	SUBSCRIBE_TO_EVENT(InputEventMouseMoved, nullptr, this, [this](const Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        onMouseMovedEventReceived();
	});
}

void UIElement::subscribeToScrollEvents()
{
	SUBSCRIBE_TO_EVENT(InputEventScroll, nullptr, this, [this](const Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        const InputEventScroll *e = (const InputEventScroll *)event;
        onScrollEventReceived(e->mScroll);
	});
}

void UIElement::subscribeToEnterEvent()
{
	SUBSCRIBE_TO_EVENT(InputEventKeyEnter, nullptr, this, [this](const Event *event)
	{
	    PROFILER_CPU()
		if (!isVisible()) { return; }
        onEnterEventReceived();
	});
}

void UIElement::subscribeToEscEvent()
{
	SUBSCRIBE_TO_EVENT(InputEventKeyEsc, nullptr, this, [this](const Event *event)
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
        GET_SYSTEM(Input).clearMouseButton();
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
        GET_SYSTEM(Input).clearMouseButton();
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

void UIElement::onScrollEventReceived(f32 scrollValue)
{
	if (!isMouseCursorInsideElement()) { return; }
    
    scroll(scrollValue);
}

void UIElement::onCharEventReceived(char character)
{
	if (!hasFocus()) { return; }

    mInputString.push_back(character);
    setText(HashedString(mInputString));
    onChar(character);
}

void UIElement::onBackspaceEventReceived()
{
	if(mInputString.empty()) { return; }

    mInputString.pop_back();
    setText(HashedString(mInputString));
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

    GET_SYSTEM(UIManager).setFocusedElement(TEntityHandler<UIElement>());
    mOnFocusLostFunctor.execute();
    onFocusLost();
}

void UIElement::requestFocus()
{
    if (hasFocus()) { return; }

    TEntityHandler<UIElement> lastFocusedElement = GET_SYSTEM(UIManager).getFocusedElement();
    if (lastFocusedElement)
    {
        lastFocusedElement->releaseFocus();
    }

    GET_SYSTEM(UIManager).setFocusedElement(EntityHandler::getEntityHandler(*this));

    mInputString.clear();
    setText(HashedString(mInputString));
    onFocus();
}

void UIElement::scroll(f32 scrollValue)
{
	mOnScrollFunctor.execute();
	onScroll(scrollValue);
}

// void UIElement::releaseOtherToggleElements()
// {
// 	const UIGroup& group = GET_SYSTEM(UIManager).getOrCreateGroup(mConfig.mGroup);
// 	FOR_LIST(it, group.getUIElements())
// 	{
// 		TEntityHandler<UIElement> other = *it;
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

    mRenderer->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = mConfig.mStyle->mColorPressed;
    mRenderer->getGPUShaderPropertiesInstance()->setDirty();
}

void UIElement::setColorRelease()
{
    if(!mRenderer) { return; }

    bool cursorInside = isMouseCursorInsideElement();
    mRenderer->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = cursorInside ? mConfig.mStyle->mColorHovered : mConfig.mStyle->mBackgroundColor;
    mRenderer->getGPUShaderPropertiesInstance()->setDirty();
}

void UIElement::setColorHover()
{
    if(!mRenderer) { return; }

    bool cursorInside = isMouseCursorInsideElement();
    mRenderer->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = cursorInside ? mConfig.mStyle->mColorHovered : mConfig.mStyle->mBackgroundColor;
    mRenderer->getGPUShaderPropertiesInstance()->setDirty();
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
            TEntityHandler<UIElement> parentUIElement = mConfig.mParent;
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
            TEntityHandler<UIElement> parentUIElement = mConfig.mParent;
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