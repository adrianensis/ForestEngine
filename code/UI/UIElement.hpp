#pragma once

#include "Scene/Module.hpp"
#include "Graphics/RenderContext.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIUtils.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIElement.hpp"

#include "Graphics/Module.hpp"

#include "Scene/Transform.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"
#include "UI/UIGroup.hpp"
#endif

class Renderer;
class UIGroup;

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

		mPressed = false;
		mConsumeInput = true;
		mOnlyReleaseOnClickOutside = false;
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
		press();
		executePressAndRelease(true);
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

		return Geometry::testRectanglePoint(
			mConfig.mPosition,
			UIUtils::correctAspectRatio_X(mConfig.mSize).x,
			mConfig.mSize.y,
			mousePosition, 0);
	}

	virtual void setText(const std::string& text) { };

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


	CPP virtual void setVisibility(bool visibility)
	{
		setIsActive(visibility);
	}

	CPP bool isVisible()
	{
		return isActive();
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
				onChar(((const InputEventChar *)event)->mChar);
			}
		});

		SUBSCRIBE_TO_EVENT(InputEventKeyBackspace, nullptr, this, [this](const Event *event)
		{
			if (isActive())
			{
				onBackspace();
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
				onMouseOver();
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
				onScroll(e->mScroll);
			}
		});
	}

	CPP void subscribeToEnterEvent()
	{
		SUBSCRIBE_TO_EVENT(InputEventKeyEnter, nullptr, this, [this](const Event *event)
		{
			if (isActive())
			{
				onFocusLost(); // TODO : call something more generic
			}
		});
	}

	CPP void subscribeToEscEvent()
	{
		SUBSCRIBE_TO_EVENT(InputEventKeyEsc, nullptr, this, [this](const Event *event)
		{
			if (isActive())
			{
				onFocusLost(); // TODO : call something more generic
			}
		});
	}

	CPP virtual void onChar(char character)
		{
		if (hasFocus())
		{
			mInputString += character;
			setText(mInputString);
		}
	}

    CPP void onBackspace()
		{
		if(!mInputString.empty())
		{
			mInputString.pop_back();
			setText(mInputString);
		}
	}

	CPP void onPressedEventReceived()
	{
		if (isVisible())
		{
			bool cursorInside = isMouseCursorInsideElement();
			
			if (cursorInside)
			{
				press();
			}
		}
	}

	CPP void onReleasedEventReceived()
	{
		if(mPressed)
		{
			if (isVisible())
			{
				if (hasFocus())
				{
					executePressAndRelease();
				}
			}
		}
	}

	virtual void onPrePressed() { };
	virtual void onPreReleased() { };
    virtual void onPostPressed() { };
	virtual void onPostReleased() { };

	CPP virtual void onMouseOver()
	{
		if(!mPressed)
		{
			if (isVisible())
			{
				if(mRenderer)
				{
					if (isMouseCursorInsideElement())
					{
						mRenderer->setColor(mConfig.mStyle->mColorHovered);
					}
					else
					{
						mRenderer->setColor(mConfig.mStyle->mBackgroundColor);
					}
				}
			}
		}
	}

	CPP virtual void onScroll(f32 scroll)
	{
		if (isVisible())
		{
			if (isMouseCursorInsideElement())
			{
				mOnScrollFunctor.execute();
			}
		}
	}

	CPP void onFocus()
	{
		mInputString.clear();
		setText(mInputString);
	}

	CPP virtual void onFocusLost()
	{
		if (hasFocus())
		{
			UIManager::getInstance().setFocusedElement(nullptr);
			mOnFocusLostFunctor.execute();
		}
	}

	CPP void press()
	{
		if(mRenderer)
		{
			mRenderer->setColor(mConfig.mStyle->mColorPressed);
		}

		mPressed = true;

		if (!hasFocus())
		{
			UIElement *lastFocusedElement = UIManager::getInstance().getFocusedElement();

			if (lastFocusedElement && lastFocusedElement->isActive())
			{
				lastFocusedElement->onFocusLost();
			}

			UIManager::getInstance().setFocusedElement(this);
			onFocus();
		}

		if (getConsumeInput())
		{
			Input::getInstance().clearMouseButton();
		}
	}

	CPP void executePressAndRelease(bool force = false)
	{
		bool cursorInside = isMouseCursorInsideElement() || force;

		bool toggleRequest = false;
		
		if(mCanToggle && cursorInside)
		{
			toggleRequest = mToggled ? false : true;
		}

		bool canExecutePress = !mCanToggle || (mCanToggle && toggleRequest);

		if(canExecutePress)
		{
			if (cursorInside)
			{
				onPrePressed();
				mOnPressedFunctor.execute();
				onPostPressed();

				if(mCanToggle)
				{
					// Release other UIToggleButtons
					const UIGroup& group = UIManager::getInstance().getOrCreateGroup(mConfig.mGroup);
					FOR_LIST(it, group.getUIElements())
					{
						UIElement* other = *it;
						if(other != this)
						{
							if(other->getCanToggle() &&
							other->getToggled() &&
							other->getReleaseOnSameGroupPressed() &&
							!other->getConfig().mGroup.empty() &&
							other->getConfig().mGroup == mConfig.mGroup)
							{
								other->release(true);
							}
						}
					}
				}
			}
		}

		/*
			NOTE:
			UIToggleButtons cannot be released by user,
			only by other UIToggleButtons.
		*/
		if(!mCanToggle || (mCanToggle && mToggled && !mReleaseOnSameGroupPressed))
		{
			release(force);
		}

		if(mCanToggle && !toggleRequest && mReleaseOnSameGroupPressed)
		{
			mToggled = true;
		}
		else
		{
			mToggled = toggleRequest;
		}
	}

	CPP void release(bool force = false)
	{
		bool cursorInside = isMouseCursorInsideElement();

		bool canExecuteRelease =
		((!mCanToggle || (mCanToggle && mToggled)) &&
		((mOnlyReleaseOnClickOutside && !cursorInside) || !mOnlyReleaseOnClickOutside)) ||
		force;

		if(canExecuteRelease)
		{
			mPressed = false;

			mToggled = false;

			onPreReleased();
			mOnReleasedFunctor.execute();

			if (getConsumeInput())
			{
				Input::getInstance().clearMouseButton();
			}

			if(mRenderer)
			{
				if(cursorInside)
				{
					mRenderer->setColor(mConfig.mStyle->mColorHovered);
				}
				else
				{
					mRenderer->setColor(mConfig.mStyle->mBackgroundColor);
				}
			}
			
			onFocusLost();

			onPostReleased();
		}
	}

protected:
	UIElementConfig mConfig;

	FunctorUIElement mOnPressedFunctor;
	FunctorUIElement mOnReleasedFunctor;

	FunctorUIElement mOnScrollFunctor;

	FunctorUIElement mOnTextChangedFunctor;
	FunctorUIElement mOnFocusLostFunctor;

	Renderer* mRenderer = nullptr;
	//PRI Collider* mCollider = nullptr; GET(Collider)
	std::string mInputString;
	bool mConsumeInput = false;
	bool mPressed = false;
	bool mCanToggle = false;
	bool mReleaseOnSameGroupPressed = false;
	bool mToggled = false;
    bool mOnlyReleaseOnClickOutside = false;

public:
	CRGET_SET(Config)
	GET(Renderer)
	GET(InputString)
	GET_SET(ConsumeInput)
	GET(Pressed)
	GET(CanToggle)
	GET_SET(ReleaseOnSameGroupPressed)
	GET(Toggled)
	GET(OnlyReleaseOnClickOutside)
};