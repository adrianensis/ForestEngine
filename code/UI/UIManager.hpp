#pragma once

#include "Core/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIFont.hpp"
#include "UI/UIShader.hpp"

class GPUShader;
class Scene;

class UIManager: public System::System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    const FontGlyphData& getGlyphData(char character) const;
    Core::WeakPtr<GPUShader> getFontShader() const;

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


    TEntityPtr<UIElement> getFocusedElement() const { return mFocusedElement; };
    void setFocusedElement(TEntityPtr<UIElement> focusedElement);

private:
	std::unordered_map<Core::HashedString, Core::OwnerPtr<UIGroup>> mGroups;
	TEntityPtr<UIElement> mFocusedElement;
    UIFontsManager mFontsManager;
    Core::HashedString mDefaultFont;
    Core::WeakPtr<GPUShader> mDefaultUIShader;

public:
    GET(DefaultUIShader)
};
REGISTER_CLASS(UIManager);
