#pragma once

#include "Core/Core.hpp"
#include "UI/UIElement.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIGroup.hpp"

class Material;
class Scene;

class UIManager: public ObjectBase, public Singleton<UIManager>
{
	GENERATE_METADATA(UIManager)
	PRI std::map<std::string, UIGroup *> mGroups;
	PRI std::map<char, Vector2> mCharMap;
	PRI Vector2 mFontTilesCount;
	PRI Vector2 mFontTileTextureSize;
	PRI Material* mFontMaterial = nullptr;

	static Vector2 smDefaultFontSize;

	PRI UIBuilder mUIBuilder;

	PRI UIElement* mFocusedElement = nullptr;

PUB
	void init();
	void terminate();

	// fonts

	const Vector2& getCharTextureCoordinates(char character);
	const Vector2& getFontTileTextureSize() const { return mFontTileTextureSize; }
	Material *getFontMaterial();
	static const Vector2& getDefaultFontSize() { return smDefaultFontSize; }

	// groups

	UIGroup& getOrCreateGroup(const std::string & groupName);

	// focus

	UIElement *getFocusedElement() const { return mFocusedElement; };
	void setFocusedElement(UIElement * focusedElement);
};