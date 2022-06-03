#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "Core/Module.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIGroup.hpp"


class Material;
class Scene;

class UIManager: public ObjectBase, public Singleton<UIManager>
{
	GENERATE_METADATA(UIManager)

public:

    void init();
    void terminate();

    const Vector2& getCharTextureCoordinates(char character);
	const Vector2& getFontTileTextureSize() const { return mFontTileTextureSize; }
    Ptr<Material> getFontMaterial();
	static const Vector2& getDefaultFontSize() { return smDefaultFontSize; }

	UIGroup& getOrCreateGroup(const String & groupName)
	{
		if (!MAP_CONTAINS(mGroups, groupName))
		{
			UIGroup *group = NEW(UIGroup);
			group->init();
			group->mName = groupName;

			MAP_INSERT(mGroups, groupName, group);
		}

		return *(mGroups.at(groupName));
	}


	UIElement *getFocusedElement() const { return mFocusedElement; };
    void setFocusedElement(UIElement * focusedElement);

private:
	std::map<String, UIGroup *> mGroups;
	std::map<char, Vector2> mCharMap;
	Vector2 mFontTilesCount;
	Vector2 mFontTileTextureSize;
	Ptr<Material> mFontMaterial;

	inline static Vector2 smDefaultFontSize = Vector2(0.04f / 2.0f, 0.04f);

	UIBuilder mUIBuilder;

	UIElement* mFocusedElement = nullptr;
};

#endif