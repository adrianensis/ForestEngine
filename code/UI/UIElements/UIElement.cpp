#include "UI/UIElements/UIElement.hpp"

#include "Graphics/Module.hpp"

#include "Scene/Transform.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"
#include "UI/UIGroup.hpp"


void UIElement::init() 
{
	GameObject::init();
}

void UIElement::initFromConfig(const UIElementConfig& config)
{
	mConfig = config;
	init();
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
		UIManager::getInstance().setFocusedElement(nullptr);
	}
}

bool UIElement::hasFocus() const
{
	return this == UIManager::getInstance().getFocusedElement();
}

bool UIElement::isMouseCursorInsideElement()
{
	Vector2 mousePosition = Input::getInstance().getMousePosition();

	if(getTransform().get().getAffectedByProjection())
	{
		mousePosition = RenderEngine::getInstance().getCamera().get().screenToWorld(Input::getInstance().getMousePosition());
	}

	return Geometry::testCubePoint(
		Rectangle(mConfig.mPosition, Vector2(UIUtils::correctAspectRatio_X(mConfig.mSize).x, mConfig.mSize.y)),
		mousePosition, 0);
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
	mRenderer = &getFirstComponent<Renderer>().get();
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
		Input::getInstance().clearMouseButton();
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
				if (isMouseCursorInsideElement())
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

void UIElement::tryRelease(bool force)
{
	if (getConsumeInput())
	{
		Input::getInstance().clearMouseButton();
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

void UIElement::focus()
{
	mInputString.clear();
	setText(mInputString);
	onFocusGained();
}

void UIElement::loseFocus()
{
	UIManager::getInstance().setFocusedElement(nullptr);
	mOnFocusLostFunctor.execute();
	onFocusLost();
}

void UIElement::obtainFocus()
{
	UIElement *lastFocusedElement = UIManager::getInstance().getFocusedElement();

	if (lastFocusedElement && lastFocusedElement->isActive())
	{
		lastFocusedElement->loseFocus();
	}

	UIManager::getInstance().setFocusedElement(this);
	focus();
}

void UIElement::scroll(f32 scrollValue)
{
	mOnScrollFunctor.execute();
	onScroll(scrollValue);
}

void UIElement::releaseOtherToggleElements()
{
	const UIGroup& group = UIManager::getInstance().getOrCreateGroup(mConfig.mGroup);
	FOR_LIST(it, group.getUIElements())
	{
		UIElement* other = *it;
		if(other != this)
		{
			if(other->getToggleEnabled() &&
			other->getState() == UIElementState::TOGGLED &&
			other->getReleaseOnSameGroupPressed() &&
			!other->getConfig().mGroup.empty() &&
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
	mRenderer->setColor(mConfig.mStyle->mColorPressed);
}

void UIElement::setColorRelease()
{
	bool cursorInside = isMouseCursorInsideElement();

	if(cursorInside)
	{
		mRenderer->setColor(mConfig.mStyle->mColorHovered);
	}
	else
	{
		mRenderer->setColor(mConfig.mStyle->mBackgroundColor);
	}
}

void UIElement::setColorOver()
{
	bool cursorInside = isMouseCursorInsideElement();

	if (isMouseCursorInsideElement())
	{
		mRenderer->setColor(mConfig.mStyle->mColorHovered);
	}
	else
	{
		mRenderer->setColor(mConfig.mStyle->mBackgroundColor);
	}
}
