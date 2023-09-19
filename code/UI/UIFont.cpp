#include "UI/UIFont.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Material/MaterialManager.hpp"

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


    // // set size to load glyphs as
    // FT_Set_Pixel_Sizes(mFreeTypeFace, 0, 48);

    _error = FT_Set_Char_Size ( mFreeTypeFace, 24 * 64, 24 * 64, 72, 72);
    CHECK_MSG(!_error, "Failed to set char size");

    // _error = FT_Set_Char_Size(
    //   mFreeTypeFace,    /* handle to face object           */
    //   0,       /* char_width in 1/64th of points  */
    //   24*64,   /* char_height in 1/64th of points */
    //   0,     /* horizontal device resolution    */
    //   0 );   /* vertical device resolution      */
    
    // For Some Twisted Reason, FreeType Measures Font Size
    // In Terms Of 1/64ths Of Pixels.  Thus, To Make A Font
    // h Pixels High, We Need To Request A Size Of h*64.
    // (h << 6 Is Just A Prettier Way Of Writing h*64)
    // u32 fontSize = 18;
    // FT_Set_Char_Size( mFreeTypeFace, fontSize << 6, fontSize << 6, 96, 96);
    // if(_error)
    // {
    //     CHECK_MSG(false, "Failed to set char size");
    // }

    CHECK_MSG(!_error, "Failed to open font");

    u32 charSetCount = 1;
    FOR_RANGE(c, 0, charSetCount)
    {
        _error = FT_Load_Char(mFreeTypeFace, c + 65, FT_LOAD_DEFAULT);
        CHECK_MSG(!_error, "Failed to load Glyph: " + c);

        _error = FT_Render_Glyph(mFreeTypeFace->glyph, FT_RENDER_MODE_NORMAL);
        CHECK_MSG(!_error, "Failed to render Glyph: " + c);

        mWidth += mFreeTypeFace->glyph->bitmap.width /*+ 2*/; // add the width of this glyph to our texture width
        // Note: We add 2 pixels of blank space between glyphs for padding - this helps reduce texture bleeding
        //       that can occur with antialiasing

        mHeight = std::max(mHeight, (u32)mFreeTypeFace->glyph->bitmap.rows);
    }

    u32 texPos = 0;
    FOR_RANGE(c, 0, charSetCount)
    {
        _error = FT_Load_Char(mFreeTypeFace, c + 65, FT_LOAD_DEFAULT);
        CHECK_MSG(!_error, "Failed to load Glyph: " + c);

        _error = FT_Render_Glyph(mFreeTypeFace->glyph, FT_RENDER_MODE_NORMAL);
        CHECK_MSG(!_error, "Failed to render Glyph: " + c);

        mGlyphs[c] = {
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            
            // The dimensions are organized as follows:
            // x: total bitmap width
            // y: total bitmap height
            // w: advancement to the next character
            // h: height offset from the top of the character to the line
            // w and h are represented in 1/64ths of a pixel so we need 
            // to convert them to accurate on-screen pixels.
            Vector2(mFreeTypeFace->glyph->advance.x >> 6, mFreeTypeFace->glyph->advance.y >> 6),
            Vector2(mFreeTypeFace->glyph->bitmap.width, mFreeTypeFace->glyph->bitmap.rows),
            Vector2(mFreeTypeFace->glyph->bitmap_left, mFreeTypeFace->glyph->bitmap_top),
            (f32)texPos / (f32)mWidth,
            nullptr
        };

        mGlyphs[c].mData = new byte[mFreeTypeFace->glyph->bitmap.width * mFreeTypeFace->glyph->bitmap.rows];

        memcpy(mGlyphs[c].mData, mFreeTypeFace->glyph->bitmap.buffer, mFreeTypeFace->glyph->bitmap.width * mFreeTypeFace->glyph->bitmap.rows);

        ImageUtils::flipImageVertically({mGlyphs[c].mData, mFreeTypeFace->glyph->bitmap.width, mFreeTypeFace->glyph->bitmap.rows}, 1);

        // Increase texture offset
        texPos += mGlyphs[c].mBitmapSize.x /*+ 2*/;
    }

    _error = FT_Done_Face(mFreeTypeFace);
    CHECK_MSG(!_error, "Failed to free font");

    MaterialData materialData;
    materialData.mCreateMipMap = false;
    //materialData.mAlphaEnabled = false;
    materialData.mFontTextureData.mPath = fontFile;
    materialData.mFontTextureData.mFontGlyphs = std::vector<TextureFontGlyph>(std::begin(mGlyphs), std::end(mGlyphs));
    materialData.mFontTextureData.mFontWidth = mWidth;
    materialData.mFontTextureData.mFontHeight = mHeight;
    mFontMaterial = GET_SYSTEM(MaterialManager).createMaterialFont(materialData);
}