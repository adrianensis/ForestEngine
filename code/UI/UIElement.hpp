#pragma once

// CPP_IGNORE

#include "Scene/Module.hpp"
#include "Graphics/RenderContext.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIUtils.hpp"

class Renderer;
class UIGroup;

class UIElement;

using UIElementCallback = std::function<void(UIElement *uiElement)>;

class FunctorUIElement: public Functor<UIElementCallback>
{
	GENERATE_METADATA(FunctorUIElement)

public:
	UIElement* mUIElement = nullptr;

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
};

class UIElement: public GameObject
{
    GENERATE_METADATA(UIElement)

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

protected:
    void subscribeToKeyEvents();
	void subscribeToCharEvents();
	void subscribeToMouseEvents();
	void subscribeToScrollEvents();
	void subscribeToEnterEvent();
	void subscribeToEscEvent();

	virtual void onChar(char character);
    void onBackspace();

    void onPressedEventReceived();
	void onReleasedEventReceived();

	virtual void onPrePressed() { };
	virtual void onPreReleased() { };
    virtual void onPostPressed() { };
	virtual void onPostReleased() { };

	virtual void onMouseOver();

	virtual void onScroll(f32 scroll);

	virtual void onFocusLost();
	void onFocus();

	void press();
	void executePressAndRelease(bool force = false);
	void release(bool force = false);

public:
	void init() override;
	virtual void initFromConfig(const UIElementConfig& config);
	virtual void onDestroy();

	void simulateClick();

	bool hasFocus() const;

	bool isMouseCursorInsideElement();

	virtual void setText(const std::string& text) { };

	void setOnPressedCallback(UIElementCallback callback)
	{
		mOnPressedFunctor.mUIElement = this;
		mOnPressedFunctor.setCallback(callback);
	}

	void setOnReleasedCallback(UIElementCallback callback)
	{
		mOnReleasedFunctor.mUIElement = this;
		mOnReleasedFunctor.setCallback(callback);
	}

	void setOnTextChangedCallback(UIElementCallback callback)
	{
		mOnTextChangedFunctor.mUIElement = this;
		mOnTextChangedFunctor.setCallback(callback);
	}

	void setOnFocusLostCallback(UIElementCallback callback)
	{
		mOnFocusLostFunctor.mUIElement = this;
		mOnFocusLostFunctor.setCallback(callback);
	}

	void setComponentsCache();

	virtual void setVisibility(bool visibility);
	bool isVisible();

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