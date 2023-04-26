#pragma once

#include "Core/Module.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIGroup.hpp"


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
	static const Vector2& getDefaultFontSize() { return smDefaultFontSize; }

	UIGroup& getOrCreateGroup(const std::string& groupName)
	{
		if (!MAP_CONTAINS(mGroups, groupName))
		{
            SharedPtr<UIGroup> group = SharedPtr<UIGroup>::newObject();
			group->init();
			group->mName = groupName;

			MAP_INSERT(mGroups, groupName, group);
		}

		return mGroups.at(groupName).get();
	}


    Ptr<UIElement> getFocusedElement() const { return mFocusedElement; };
    void setFocusedElement(Ptr<UIElement> focusedElement);

private:
	std::unordered_map<std::string, SharedPtr<UIGroup>> mGroups;
	std::unordered_map<char, Vector2> mCharMap;
	Vector2 mFontTilesCount;
	Vector2 mFontTileTextureSize;
	Ptr<const Material> mFontMaterial;

	inline static Vector2 smDefaultFontSize = Vector2(0.04f / 2.0f, 0.04f);

	UIBuilder mUIBuilder;

	Ptr<UIElement> mFocusedElement;
};
