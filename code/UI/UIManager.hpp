#pragma once

#include "Core/System/SystemsDependencyInjection.hpp"
#include "Engine/Core.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIFont.hpp"
#include "UI/UIShader.hpp"
#include "UI/UIBuilder.hpp"
#include "Window/WindowManager.hpp"

class GPUShader;
class Scene;

class UIManager: public System::System
{
public:
    void init();
    virtual void terminate() override;

    const Font::FontGlyphData& getGlyphData(char character) const;
    Core::WeakPtr<GPUShader> getFontShader() const;
    Core::WeakPtr<const UIFont> getFont() const;

	UIGroup& getOrCreateGroup(Core::HashedString groupName)
	{
		if (!mGroups.contains(groupName))
		{
			mGroups.insert_or_assign(groupName, Core::OwnerPtr<UIGroup>::newObject());
            Core::WeakPtr<UIGroup> group = mGroups.at(groupName);
            group->init();
            group->mName = groupName;
		}

		return mGroups.at(groupName).get();
	}

    UIElement* getFocusedElement() const { return mFocusedElement; };
    void setFocusedElement(UIElement* focusedElement);

    UIBuilder createUIBuilder()
    {
        return UIBuilder(this);
    }

private:
	std::unordered_map<Core::HashedString, Core::OwnerPtr<UIGroup>> mGroups;
	UIElement* mFocusedElement  = nullptr;
    UIFontsManager mFontsManager;
    Core::HashedString mDefaultFont;
    Core::WeakPtr<GPUShader> mDefaultUIShader;
    Window::Window* mWindow = nullptr;

public:
    GET(DefaultUIShader)
    GET(Window)
};
REGISTER_CLASS(UIManager, System);
