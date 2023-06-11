#include "UI/UIElements/UIElement.hpp"

#include "Graphics/Module.hpp"

#include "Scene/Transform.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"
#include "UI/UIGroup.hpp"

void UIElement::initFromConfig(const UIElementConfig& config)
{
	mConfig = config;
}

void UIElement::onDestroy()
{
	GameObject::onDestroy();
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
		GET_SYSTEM(UIManager).setFocusedElement(Ptr<UIElement>());
	}
}

bool UIElement::hasFocus() const
{
	return getPtrToThis() == GET_SYSTEM(UIManager).getFocusedElement();
}

bool UIElement::isMouseCursorInsideElement() const
{
    bool parentCheck = true;
    if(mConfig.mParent)
    {
        if(Ptr<UIElement> parentUIElement = Ptr<UIElement>::cast(mConfig.mParent))
        {
            parentCheck = parentUIElement->isMouseCursorInsideElement();
        }
    }

    if(!parentCheck)
    {
        return false;
    }

	Vector2 mousePosition = GET_SYSTEM(Input).getMousePosition();

	if(mTransform->mAffectedByProjection)
	{
		mousePosition = GET_SYSTEM(RenderEngine).mCamera->screenToWorld(GET_SYSTEM(Input).getMousePosition());
	}

    Vector2 correctedSize = UIUtils::correctAspectRatio_X(mConfig.mSize);
	return Geometry::testCubePoint(
		Rectangle(getLeftTopPosition(), correctedSize),
		mousePosition, 0);
}

Vector3 UIElement::getLeftTopPosition() const
{
    Vector3 position = mTransform->getWorldPosition();
    Vector2 correctedSize = UIUtils::correctAspectRatio_X(mConfig.mSize);
    position.x = position.x - (correctedSize.x / 2.0f);
    position.y = position.y + (correctedSize.y / 2.0f);

    return position;
}

void UIElement::setVisibility(bool visibility)
{
	setIsActive(visibility);
}

bool UIElement::isVisible()
{
	return isActive();
}

void UIElement::setOnPressedCallback(UIElementCallback callback)
{
	mOnPressedFunctor.mUIElement = this;
	mOnPressedFunctor.setCallback(callback);
}

void UIElement::setOnReleasedCallback(UIElementCallback callback)
{
	mOnReleasedFunctor.mUIElement = this;
	mOnReleasedFunctor.setCallback(callback);
}

void UIElement::setOnTextChangedCallback(UIElementCallback callback)
{
	mOnTextChangedFunctor.mUIElement = this;
	mOnTextChangedFunctor.setCallback(callback);
}

void UIElement::setOnFocusLostCallback(UIElementCallback callback)
{
	mOnFocusLostFunctor.mUIElement = this;
	mOnFocusLostFunctor.setCallback(callback);
}

void UIElement::setComponentsCache()
{
	mRenderer = &getFirstComponent<MeshRenderer>().get();
}

void UIElement::subscribeToKeyEvents()
{
	SUBSCRIBE_TO_EVENT(InputEventKeyPressed, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
		}
	});

	SUBSCRIBE_TO_EVENT(InputEventKeyReleased, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
		}
	});
}

void UIElement::subscribeToCharEvents()
{
	SUBSCRIBE_TO_EVENT(InputEventChar, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
			onCharEventReceived(((const InputEventChar *)event)->mChar);
		}
	});

	SUBSCRIBE_TO_EVENT(InputEventKeyBackspace, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
			onBackspaceEventReceived();
		}
	});
}

void UIElement::subscribeToMouseEvents()
{
	SUBSCRIBE_TO_EVENT(InputEventMouseButtonPressed, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
			const InputEventMouseButtonPressed *e = (const InputEventMouseButtonPressed *)event;

			if (e->mButton == GLFW_MOUSE_BUTTON_LEFT)
			{
				onPressedEventReceived();
			}
		}
	});

	SUBSCRIBE_TO_EVENT(InputEventMouseButtonReleased, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
			const InputEventMouseButtonReleased *e = (const InputEventMouseButtonReleased *)event;
			if (e->mButton == GLFW_MOUSE_BUTTON_LEFT)
			{
				onReleasedEventReceived();
			}
		}
	});

	SUBSCRIBE_TO_EVENT(InputEventMouseMoved, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
			onMouseOverEventReceived();
		}
	});
}

void UIElement::subscribeToScrollEvents()
{
	SUBSCRIBE_TO_EVENT(InputEventScroll, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
			const InputEventScroll *e = (const InputEventScroll *)event;
			onScrollEventReceived(e->mScroll);
		}
	});
}

void UIElement::subscribeToEnterEvent()
{
	SUBSCRIBE_TO_EVENT(InputEventKeyEnter, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
			onEnterEventReceived();
		}
	});
}

void UIElement::subscribeToEscEvent()
{
	SUBSCRIBE_TO_EVENT(InputEventKeyEsc, nullptr, this, [this](const Event *event)
	{
		if (isActive())
		{
			onEscEventReceived();
		}
	});
}

void UIElement::onPressedEventReceived()
{
	if (isVisible())
	{
        bool cursorInside = isMouseCursorInsideElement();
            
        if (cursorInside)
        {
            markAsPressed();
        }
	}
}

void UIElement::executePressed()
{
	onPrePressed();
	mOnPressedFunctor.execute();
	onPostPressed();
}

void UIElement::markAsPressed()
{
	setColorPressed();

	if(mState == UIElementState::RELEASED)
	{
		mState = UIElementState::PRESSED;
	}

	if (!hasFocus())
	{
		obtainFocus();
	}

	if (getConsumeInput())
	{
		GET_SYSTEM(Input).clearMouseButton();
	}
}

void UIElement::onReleasedEventReceived()
{
	if(mState == UIElementState::PRESSED)
	{
		if (isVisible())
		{
			if (hasFocus())
			{
                bool cursorInside = isMouseCursorInsideElement();
                if(mOnlyReleaseOnClickOutside)
                {
                    if(!cursorInside)
                    {
                        tryRelease(false);
                    }
                }
                else
                {
                    if (cursorInside)
                    {
                        executePressed();
                        tryRelease(false);
                    }
                    else
                    {
                        if(!mToggleEnabled || (mState != UIElementState::TOGGLED))
                        {
                            markAsReleased();
                        }
                    }
                }
			}
		}
	}
}

void UIElement::tryRelease(bool force)
{
	if (getConsumeInput())
	{
		GET_SYSTEM(Input).clearMouseButton();
	}

	bool shouldRelease = false;

	if(mToggleEnabled)
	{
		if(mState == UIElementState::TOGGLED)
		{
			if(!mReleaseOnSameGroupPressed)
			{
				shouldRelease = true;
			}
		}
		else
		{
			markAsToggled();
		}
	}
	else
	{
		shouldRelease = true;
	}

	if(shouldRelease)
	{
		executeRelease();
	}
}

void UIElement::executeRelease()
{
	onPreReleased();
	markAsReleased();
	mOnReleasedFunctor.execute();
	onPostReleased();
}

void UIElement::markAsReleased()
{
	mState = UIElementState::RELEASED;
	setColorRelease();
	if (hasFocus())
	{
		loseFocus();
	}
}

void UIElement::onMouseOverEventReceived()
{
	if(mState == UIElementState::RELEASED)
	{
		if (isVisible())
		{
			if(mRenderer)
			{
				setColorOver();

				if (isMouseCursorInsideElement())
				{
					onMouseOverEnter();
				}
				else
				{
					onMouseOverExit();
				}
			}
		}
	}
}

void UIElement::onScrollEventReceived(f32 scrollValue)
{
	if (isVisible())
	{
		if (isMouseCursorInsideElement())
		{
			scroll(scrollValue);
		}
	}
}

void UIElement::onCharEventReceived(char character)
{
	if (hasFocus())
	{
		mInputString += character;
		setText(mInputString);
		onChar(character);
	}
}

void UIElement::onBackspaceEventReceived()
{
	if(!mInputString.empty())
	{
		mInputString.pop_back();
		setText(mInputString);
		onBackspace();
	}
}

void UIElement::onEnterEventReceived()
{
	if (hasFocus())
	{
		onEnter();
		loseFocus();
	}
}

void UIElement::onEscEventReceived()
{
	if (hasFocus())
	{
		onEsc();
		loseFocus();
	}
}

void UIElement::loseFocus()
{
	GET_SYSTEM(UIManager).setFocusedElement(Ptr<UIElement>());
	mOnFocusLostFunctor.execute();
	onFocusLost();
}

void UIElement::obtainFocus()
{
	Ptr<UIElement> lastFocusedElement = GET_SYSTEM(UIManager).getFocusedElement();
	if (lastFocusedElement and lastFocusedElement->isActive())
	{
		lastFocusedElement->loseFocus();
	}

	GET_SYSTEM(UIManager).setFocusedElement(getPtrToThis());

	mInputString.clear();
	setText(mInputString);
	onFocus();
}

void UIElement::scroll(f32 scrollValue)
{
	mOnScrollFunctor.execute();
	onScroll(scrollValue);
}

void UIElement::releaseOtherToggleElements()
{
	const UIGroup& group = GET_SYSTEM(UIManager).getOrCreateGroup(mConfig.mGroup);
	FOR_LIST(it, group.getUIElements())
	{
		Ptr<UIElement> other = *it;
		if(other != getPtrToThis())
		{
			if(other->getToggleEnabled() and
			other->getState() == UIElementState::TOGGLED and
			other->getReleaseOnSameGroupPressed() and
			!other->getConfig().mGroup.empty() and
			other->getConfig().mGroup == mConfig.mGroup)
			{
				other->executeRelease();
			}
		}
	}
}

void UIElement::markAsToggled()
{
	mState = UIElementState::TOGGLED;
	releaseOtherToggleElements();
}

void UIElement::setColorPressed()
{
    if(mRenderer)
    {
	    mRenderer->mColor = (mConfig.mStyle->mColorPressed);
    }
}

void UIElement::setColorRelease()
{
	bool cursorInside = isMouseCursorInsideElement();
	if(cursorInside)
	{
        if(mRenderer)
        {
		    mRenderer->mColor = (mConfig.mStyle->mColorHovered);
        }
	}
	else
	{
        if(mRenderer)
        {
		    mRenderer->mColor = (mConfig.mStyle->mBackgroundColor);
        }
	}
}

void UIElement::setColorOver()
{
	bool cursorInside = isMouseCursorInsideElement();
	if (cursorInside)
	{
		mRenderer->mColor = (mConfig.mStyle->mColorHovered);
	}
	else
	{
		mRenderer->mColor = (mConfig.mStyle->mBackgroundColor);
	}
}
