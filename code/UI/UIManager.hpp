#pragma once

#include "Core/Module.hpp"
#include "UI/UIElement.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIGroup.hpp"

class Material;
class Scene;

class UIManager: public ObjectBase, public Singleton<UIManager>
{
	GENERATE_METADATA(UIManager)

private:
	std::map<std::string, UIGroup *> mGroups;
	std::map<char, Vector2> mCharMap;
	Vector2 mFontTilesCount;
	Vector2 mFontTileTextureSize;
	Material* mFontMaterial = nullptr;

	static Vector2 smDefaultFontSize;

	UIBuilder mUIBuilder;

	UIElement* mFocusedElement = nullptr;

public:
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