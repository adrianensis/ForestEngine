#pragma once

#include "Scene/Module.hpp"
#include "Graphics/RenderContext.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIUtils.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIElements/UIElement.hpp"

#include "Graphics/Module.hpp"

#include "Scene/Transform.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"
#include "UI/UIGroup.hpp"
#endif

class Renderer;
class UIGroup;

enum class UIElementState
{
	PRESSED, RELEASED, TOGGLED
};

class UIElement;

using UIElementCallback = std::function<void(UIElement *uiElement)>;

class FunctorUIElement: public Functor<UIElementCallback>
{
	GENERATE_METADATA(FunctorUIElement)

public:
	void execute() override
	{
		if (mCallback)
		{
			mCallback(mUIElement);
		}
	}

	COPY(FunctorUIElement)
	{
		DO_COPY(mUIElement)
		Functor<UIElementCallback>::copy(other);
	}

public:
	UIElement* mUIElement = nullptr;
};

class UIElement: public GameObject
{
    GENERATE_METADATA(UIElement)

public:
	CPP	void init() override
	{
		GameObject::init();
	}

	CPP virtual void initFromConfig(const UIElementConfig& config)
	{
		mConfig = config;
		init();
	}

	CPP virtual void onDestroy()
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

	void simulateClick()
	{
		markAsPressed();
		executePressed();
		tryRelease(true);
	}

	CPP bool hasFocus() const
	{
		return this == UIManager::getInstance().getFocusedElement();
	}

	CPP bool isMouseCursorInsideElement()
	{
		//collider->getBoundingBox(true); // force regenerate bounding box
		Vector2 mousePosition = Input::getInstance().getMousePosition();

		if(getTransform().get().getAffectedByProjection())
		{
			mousePosition = RenderEngine::getInstance().getCamera().get().screenToWorld(Input::getInstance().getMousePosition());
		}

		return Geometry::testCubePoint(
			Rectangle(mConfig.mPosition, Vector2(UIUtils::correctAspectRatio_X(mConfig.mSize).x, mConfig.mSize.y)),
			mousePosition, 0);
	}

	virtual void setText(const std::string& text) { };

	CPP virtual void setVisibility(bool visibility)
	{
		setIsActive(visibility);
	}

	CPP bool isVisible()
	{
		return isActive();
	}

	CPP void setOnPressedCallback(UIElementCallback callback)
	{
		mOnPressedFunctor.mUIElement = this;
		mOnPressedFunctor.setCallback(callback);
	}

	CPP void setOnReleasedCallback(UIElementCallback callback)
	{
		mOnReleasedFunctor.mUIElement = this;
		mOnReleasedFunctor.setCallback(callback);
	}

	CPP void setOnTextChangedCallback(UIElementCallback callback)
	{
		mOnTextChangedFunctor.mUIElement = this;
		mOnTextChangedFunctor.setCallback(callback);
	}

	CPP void setOnFocusLostCallback(UIElementCallback callback)
	{
		mOnFocusLostFunctor.mUIElement = this;
		mOnFocusLostFunctor.setCallback(callback);
	}

	CPP void setComponentsCache()
	{
		mRenderer = &getFirstComponent<Renderer>().get();
		//mCollider = getFirstComponent<Collider>();
	}


protected:
	CPP void subscribeToKeyEvents()
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

	CPP void subscribeToCharEvents()
	{
		SUBSCRIBE_TO_EVENT(InputEventChar, nullptr, this, [this](const Event *event)
		{
			if (isActive())
			{
				// TODO : boolean to enable or disable : can receive char input?
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

	CPP void subscribeToMouseEvents()
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
				//const InputEventMouseMoved *e = (const InputEventMouseMoved *)event;
				onMouseOverEventReceived();
			}
		});
	}

	CPP void subscribeToScrollEvents()
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

	CPP void subscribeToEnterEvent()
	{
		SUBSCRIBE_TO_EVENT(InputEventKeyEnter, nullptr, this, [this](const Event *event)
		{
			if (isActive())
			{
				onEnterEventReceived();
			}
		});
	}

	CPP void subscribeToEscEvent()
	{
		SUBSCRIBE_TO_EVENT(InputEventKeyEsc, nullptr, this, [this](const Event *event)
		{
			if (isActive())
			{
				onEscEventReceived();
			}
		});
	}

private:
	
	// ########### PRESSED ###########

	CPP void onPressedEventReceived()
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

	CPP void executePressed()
	{
		onPrePressed();
		mOnPressedFunctor.execute();
		onPostPressed();
	}


	CPP void markAsPressed()
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

	// ########### RELEASED ###########

	CPP void onReleasedEventReceived()
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

	CPP void tryRelease(bool force)
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

	CPP void executeRelease()
	{
		onPreReleased();
		markAsReleased();
		mOnReleasedFunctor.execute();
		onPostReleased();
	}

	CPP void markAsReleased()
	{
		mState = UIElementState::RELEASED;
		setColorRelease();
		if (hasFocus())
		{
			loseFocus();
		}
	}

	CPP void onMouseOverEventReceived()
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

	CPP void onScrollEventReceived(f32 scrollValue)
	{
		if (isVisible())
		{
			if (isMouseCursorInsideElement())
			{
				scroll(scrollValue);
			}
		}
	}


	CPP void onCharEventReceived(char character)
	{
		if (hasFocus())
		{
			mInputString += character;
			setText(mInputString);
			onChar(character);
		}
	}

    CPP void onBackspaceEventReceived()
	{
		if(!mInputString.empty())
		{
			mInputString.pop_back();
			setText(mInputString);
			onBackspace();
		}
	}

	CPP void onEnterEventReceived()
	{
		if (hasFocus())
		{
			onEnter();
			loseFocus();
		}
	}

	CPP void onEscEventReceived()
	{
		if (hasFocus())
		{
			onEsc();
			loseFocus();
		}
	}

	// ########### FOCUS ###########

	CPP void focus()
	{
		mInputString.clear();
		setText(mInputString);
		onFocusGained();
	}

	CPP void loseFocus()
	{
		UIManager::getInstance().setFocusedElement(nullptr);
		mOnFocusLostFunctor.execute();
		onFocusLost();
	}

	CPP void obtainFocus()
	{
		UIElement *lastFocusedElement = UIManager::getInstance().getFocusedElement();

		if (lastFocusedElement && lastFocusedElement->isActive())
		{
			lastFocusedElement->loseFocus();
		}

		UIManager::getInstance().setFocusedElement(this);
		focus();
	}

	// ########### SCROLL ###########
	
	CPP void scroll(f32 scrollValue)
	{
		mOnScrollFunctor.execute();
		onScroll(scrollValue);
	}

	// ########### TOGGLE ###########

	CPP void releaseOtherToggleElements()
	{
		// Release other UIToggleButtons
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

	CPP void markAsToggled()
	{
		mState = UIElementState::TOGGLED;
		releaseOtherToggleElements();
	}

	// ########### COLOR ###########

	CPP void setColorPressed()
	{
		mRenderer->setColor(mConfig.mStyle->mColorPressed);
	}

	CPP void setColorRelease()
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

	CPP void setColorOver()
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

protected:
	virtual void onPrePressed() { }
	virtual void onPreReleased() { }
    virtual void onPostPressed() { }
	virtual void onPostReleased() { }
	virtual void onChar(char character) { }
    virtual void onBackspace() { }
	virtual void onEnter() { }
	virtual void onEsc() { }
	virtual void onMouseOverEnter() { }
	virtual void onMouseOverExit() { }
	virtual void onScroll(f32 scroll) { }
	virtual void onFocusGained() { }
	virtual void onFocusLost() { }

protected:
	UIElementConfig mConfig;

	FunctorUIElement mOnPressedFunctor;
	FunctorUIElement mOnReleasedFunctor;

	FunctorUIElement mOnScrollFunctor;

	FunctorUIElement mOnTextChangedFunctor;
	FunctorUIElement mOnFocusLostFunctor;

	Renderer* mRenderer = nullptr;
	std::string mInputString;
	bool mConsumeInput = true;
	UIElementState mState = UIElementState::RELEASED;
	bool mToggleEnabled = false;
	bool mReleaseOnSameGroupPressed = false;
    bool mOnlyReleaseOnClickOutside = false;

public:
	CRGET_SET(Config)
	GET(Renderer)
	GET(InputString)
	GET_SET(ConsumeInput)
	GET(ToggleEnabled)
	GET_SET(ReleaseOnSameGroupPressed)
	GET(State)
	GET(OnlyReleaseOnClickOutside)
};