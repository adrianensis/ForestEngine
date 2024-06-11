#pragma once

#include "Scene/Module.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIUtils.hpp"
#include "Engine/Input/Input.hpp"

class MeshRenderer;
class UIGroup;

enum class UIElementState
{
	PRESSED, RELEASED, //TOGGLED
};

class UIElement;

using UIElementCallback = std::function<void(UIElement *uiElement)>;

class FunctorUIElement: public Functor<UIElementCallback>
{
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
public:
    virtual void initFromConfig(const UIElementConfig& config);
    void postInit();
    virtual UIElementConfig calculateConfig(const UIElementConfig& config) { return config; };
    virtual void onDestroy();

    bool hasFocus() const;
    bool isMouseCursorInsideElement() const;
    Vector3 getLeftTopPosition() const;

	virtual void setText(HashedString text) { };
    virtual void setVisibility(bool visibility);
    bool isVisible();
    void setOnPressedCallback(UIElementCallback callback);
    void setOnTextChangedCallback(UIElementCallback callback);
    void setOnFocusLostCallback(UIElementCallback callback);

protected:
    void subscribeToKeyEvents();
    void subscribeToCharEvents();
    void subscribeToMouseEvents();
    void subscribeToScrollEvents();
    void subscribeToEnterEvent();
    void subscribeToEscEvent();

private:
    void onPressedEventReceived();
    void onReleasedEventReceived();
    void onMouseMovedEventReceived();
    void onScrollEventReceived(f32 scrollValue);

    void onCharEventReceived(char character);
    void onBackspaceEventReceived();
    void onEnterEventReceived();
    void onEscEventReceived();

    void releaseFocus();
    void requestFocus();

    void scroll(f32 scrollValue);

    // void releaseOtherToggleElements();

    void setColorPressed();
    void setColorRelease();
    void setColorHover();

protected:
	virtual void onPrePressed() { }
    virtual void onPostPressed() { }
	virtual void onChar(char character) { }
    virtual void onBackspace() { }
	virtual void onEnter() { }
	virtual void onEsc() { }
	virtual void onMouseOverEnter() { }
	virtual void onMouseOverExit() { }
	virtual void onScroll(f32 scroll) { }
	virtual void onFocus() { }
	virtual void onFocusLost() { }

    StencilData calculateStencilData() const;

protected:
	UIElementConfig mConfig;

	FunctorUIElement mOnPressedFunctor;
	FunctorUIElement mOnScrollFunctor;
	FunctorUIElement mOnTextChangedFunctor;
	FunctorUIElement mOnFocusLostFunctor;

	Ptr<MeshRenderer> mRenderer;
	std::string mInputString;
	bool mConsumeInput = true;
	UIElementState mState = UIElementState::RELEASED;
    bool mOnlyReleaseOnClickOutside = false;
    
    bool mClipChildren = false;

public:
	CRGET_SET(Config)
	GET(InputString)
	GET(State)
	GET(OnlyReleaseOnClickOutside)
};
REGISTER_CLASS(UIElement);
