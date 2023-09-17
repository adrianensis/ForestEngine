#pragma once

#include "Core/Module.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIFont.hpp"

class Material;
class Scene;

class UIManager: public EngineSystem
{
	GENERATE_METADATA(UIManager)
public:

    void init();
    void terminate();

    const Vector2& getCharTextureCoordinates(char character);
	const Vector2& getFontTileTextureSize() const { return mFontTileTextureSize; }
    Ptr<const Material> getFontMaterial();

	UIGroup& getOrCreateGroup(const std::string& groupName)
	{
		if (!MAP_CONTAINS(mGroups, groupName))
		{
			MAP_INSERT(mGroups, groupName, OwnerPtr<UIGroup>::newObject());
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
	std::unordered_map<char, Vector2> mCharMap;
	Vector2 mFontTilesCount;
	Vector2 mFontTileTextureSize;
	Ptr<const Material> mFontMaterial;

	Vector2 mDefaultFontSize = Vector2(0.04f / 2.0f, 0.04f);

	Ptr<UIElement> mFocusedElement;

    UIFontsManager mFontsManager;

public:
    CRGET(DefaultFontSize)
};
