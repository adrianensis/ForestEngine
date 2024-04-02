#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/Systems/System.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIFont.hpp"
#include "UI/UIMaterial.hpp"

class Material;
class Scene;

class UIManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    const FontGlyphData& getGlyphData(char character) const;
    PoolHandler<Material> getFontMaterial() const;

	UIGroup& getOrCreateGroup(const std::string& groupName)
	{
		if (!mGroups.contains(groupName))
		{
			mGroups.insert_or_assign(groupName, OwnerPtr<UIGroup>::newObject());
            Ptr<UIGroup> group = mGroups.at(groupName);
            group->init();
            group->mName = groupName;
		}

		return mGroups.at(groupName).get();
	}


    Ptr<UIElement> getFocusedElement() const { return mFocusedElement; };
    void setFocusedElement(Ptr<UIElement> focusedElement);

private:
	std::unordered_map<std::string, OwnerPtr<UIGroup>> mGroups;
	Ptr<UIElement> mFocusedElement;
    UIFontsManager mFontsManager;
    std::string mDefaultFont;
    PoolHandler<Material> mDefaultUIMaterial;

public:
    GET(DefaultUIMaterial)
};
REGISTER_CLASS(UIManager);
