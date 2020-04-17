#ifndef DE_UI_H
#define DE_UI_H

#include "DE_Class.h"
#include "Singleton.h"
#include "Vector2.h"
#include "UIElement.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIList.h"

#include <string>

namespace DE {

class Texture;
class Material;
class Scene;

template <class T> class List;
template <class K, class V> class HashMap;

class UI : public DE_Class, public Singleton<UI> {
private:

Texture* mButtonTexture;
Material* mButtonMaterial;

Texture* mFontTexture;
Material* mFontMaterial;

List<UIElement*>* mUIElements;
HashMap<c8, Vector2>* mCharMap;

f32 mFontTilesCount;
f32 mFontTileTextureSize;

public:

DE_CLASS(UI, DE_Class);

UIButton* createButton(Scene* scene, const Vector2& position, const Vector2& size, u32 layer);
UIText* createText(Scene* scene, const Vector2& position, const Vector2& size, const std::string& text, u32 layer);
UIList* createList(Scene* scene, const Vector2& position, const Vector2& size, u32 layer);

Vector2 getCharTextureCoordinates(c8 character);
f32 getFontTileTextureSize() const { return mFontTileTextureSize; };
Material* getFontMaterial() const { return mFontMaterial; };

void init();
void step();
void terminate();

};

} /* namespace DE */

#endif /* DE_UI_H */