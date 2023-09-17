#include "UI/UIFont.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

void UIFontsManager::init()
{
    FT_Error _error;
    _error = FT_Init_FreeType(&mFreeTypeLibrary);
    CHECK_MSG(!_error, "Failed to initialize FreeType");
}

void UIFontsManager::terminate()
{
    mFontsMap.clear();

    FT_Error _error;
    _error = FT_Done_FreeType(mFreeTypeLibrary);
    CHECK_MSG(!_error, "Failed to free FreeType");
}

Ptr<UIFont> UIFontsManager::loadFont(const std::string& fontFile)
{
    if(!MAP_CONTAINS(mFontsMap, fontFile))
    {
        MAP_INSERT(mFontsMap, fontFile, OwnerPtr<UIFont>::newObject());
		Ptr<UIFont> font = mFontsMap.at(fontFile);
		font->init(*this, fontFile);
    }

    return mFontsMap.at(fontFile);
}

UIFont::~UIFont()
{
    FT_Error _error;
    _error = FT_Done_Face(mFreeTypeFace);
    CHECK_MSG(!_error, "Failed to free font");
}

void UIFont::init(UIFontsManager& fontsManager, const std::string& fontFile)
{
    mPath = fontFile;

    // Create a new font
    FT_Error _error;
    _error = FT_New_Face(fontsManager.getFreeTypeLibrary(),       // FreeType instance handle
                         mPath.c_str(), // Font family to use
                         0,         // index of font (in case there are more than one in the file)
                         &mFreeTypeFace);   // font face handle

    if(_error == FT_Err_Unknown_File_Format)
    {
        CHECK_MSG(false, "Failed to open font: unknown font format");
    }

    CHECK_MSG(!_error, "Failed to open font");

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(mFreeTypeFace, 0, 48);

    int rowWidth = 0;
    int colHeight = 0;

    for(int i = 0; i < 128; ++i) {
        if(FT_Load_Char(mFreeTypeFace, i, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue; // try next character
        }

        mWidth += mFreeTypeFace->glyph->bitmap.width + 2; // add the width of this glyph to our texture width
        // Note: We add 2 pixels of blank space between glyphs for padding - this helps reduce texture bleeding
        //       that can occur with antialiasing

        mHeight = std::max(mHeight, (u32)mFreeTypeFace->glyph->bitmap.rows);
    }

    u32 textureId = GET_SYSTEM(GPUInterface).createTextureFont(mWidth, mHeight, GL_RED, nullptr);

    int texPos = 0;

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(mFreeTypeFace, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        mGlyphs[c] = {
            Vector2(mFreeTypeFace->glyph->advance.x >> 6, mFreeTypeFace->glyph->advance.y >> 6),
            Vector2(mFreeTypeFace->glyph->bitmap.width, mFreeTypeFace->glyph->bitmap.rows),
            Vector2(mFreeTypeFace->glyph->bitmap_left, mFreeTypeFace->glyph->bitmap_top),
            mGlyphs[c].xOffset = (f32)texPos / (f32)mWidth
        };

        GET_SYSTEM(GPUInterface).subTexture(texPos, 0, 1, mFreeTypeFace->glyph->bitmap.rows, GL_RED, nullptr);
        GET_SYSTEM(GPUInterface).subTexture(texPos, 0, mFreeTypeFace->glyph->bitmap.width, mFreeTypeFace->glyph->bitmap.rows, GL_RED, mFreeTypeFace->glyph->bitmap.buffer);
        GET_SYSTEM(GPUInterface).subTexture(texPos, 0, 1, mFreeTypeFace->glyph->bitmap.rows, GL_RED, nullptr);
    }
    GET_SYSTEM(GPUInterface).disableTexture();
}