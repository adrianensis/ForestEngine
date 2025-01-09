#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIFont.hpp"
#include "UI/UIShader.hpp"

class Shader;
class Scene;

class UIManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    const FontGlyphData& getGlyphData(char character) const;
    WeakPtr<Shader> getFontShader() const;

	UIGroup& getOrCreateGroup(HashedString groupName)
	{
		if (!mGroups.contains(groupName))
		{
			mGroups.insert_or_assign(groupName, OwnerPtr<UIGroup>::newObject());
            WeakPtr<UIGroup> group = mGroups.at(groupName);
            group->init();
            group->mName = groupName;
		}

		return mGroups.at(groupName).get();
	}


    TEntityHandler<UIElement> getFocusedElement() const { return mFocusedElement; };
    void setFocusedElement(TEntityHandler<UIElement> focusedElement);

private:
	std::unordered_map<HashedString, OwnerPtr<UIGroup>> mGroups;
	TEntityHandler<UIElement> mFocusedElement;
    UIFontsManager mFontsManager;
    HashedString mDefaultFont;
    WeakPtr<Shader> mDefaultUIShader;

public:
    GET(DefaultUIShader)
};
REGISTER_CLASS(UIManager);
