#pragma once

#include "Scene/Module.hpp"
#include "Graphics/RenderContext.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIUtils.hpp"


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

public:
	UIElement* mUIElement = nullptr;
};

class UIElement: public GameObject
{
    GENERATE_METADATA(UIElement)

public:
    void init() override;
    virtual void initFromConfig(const UIElementConfig& config);
    virtual void onDestroy();

	void simulateClick()
	{
		markAsPressed();
		executePressed();
		tryRelease(true);
	}

    bool hasFocus() const;
    bool isMouseCursorInsideElement() const;
    Vector3 getLeftTopPosition() const;

	virtual void setText(const std::string& text) { };
    virtual void setVisibility(bool visibility);
    bool isVisible();
    void setOnPressedCallback(UIElementCallback callback);
    void setOnReleasedCallback(UIElementCallback callback);
    void setOnTextChangedCallback(UIElementCallback callback);
    void setOnFocusLostCallback(UIElementCallback callback);
    void setComponentsCache();


protected:
    void subscribeToKeyEvents();
    void subscribeToCharEvents();
    void subscribeToMouseEvents();
    void subscribeToScrollEvents();
    void subscribeToEnterEvent();
    void subscribeToEscEvent();

private:
	
    void onPressedEventReceived();
    void executePressed();

    void markAsPressed();

    void onReleasedEventReceived();
    void tryRelease(bool force);
    void executeRelease();
    void markAsReleased();
    void onMouseOverEventReceived();
    void onScrollEventReceived(f32 scrollValue);

    void onCharEventReceived(char character);
    void onBackspaceEventReceived();
    void onEnterEventReceived();
    void onEscEventReceived();

    void loseFocus();
    void obtainFocus();

    void scroll(f32 scrollValue);

    void releaseOtherToggleElements();
    void markAsToggled();

    void setColorPressed();
    void setColorRelease();
    void setColorOver();

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
	virtual void onFocus() { }
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
