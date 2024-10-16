#include "Core/Font/Font.hpp"
#include "Core/Image/ImageUtils.hpp"

void FontsLibrary::init()
{
    FT_Error _error;
    _error = FT_Init_FreeType(&mFreeTypeLibrary);
    CHECK_MSG(!_error, "Failed to initialize FreeType");
}

void FontsLibrary::terminate()
{
    FT_Error _error;
    _error = FT_Done_FreeType(mFreeTypeLibrary);
    CHECK_MSG(!_error, "Failed to free FreeType");
}

void FontGlyphData::freeBuffer()
{
    delete[] mData;
    mData = nullptr;
}

void FontData::loadFont(FontsLibrary& fontsLibrary, HashedString fontFile, u32 fontSize)
{
    mPath = fontFile;

    CHECK_MSG(fontSize > 0, "Font Size must be greater than 0");

    // Create a new font
    FT_Error _error;
    _error = FT_New_Face(fontsLibrary.getFreeTypeLibrary(),       // FreeType instance handle
                         mPath.get().c_str(), // Font family to use
                         0,         // index of font (in case there are more than one in the file)
                         &mFreeTypeFace);   // font face handle

    if(_error == FT_Err_Unknown_File_Format)
    {
        CHECK_MSG(false, "Failed to open font: unknown font format");
    }


    // // set size to load glyphs as
    // FT_Set_Pixel_Sizes(mFreeTypeFace, 0, 48);

    _error = FT_Set_Char_Size ( mFreeTypeFace, 0, fontSize * 64, 300, 300);
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

    u32 charSetCount = MAX_GLYPHS;
    FOR_RANGE(c, 0, charSetCount)
    {
        _error = FT_Load_Char(mFreeTypeFace, c, FT_LOAD_DEFAULT);
        CHECK_MSG(!_error, "Failed to load Glyph: " + std::to_string(c));

        mWidth += mFreeTypeFace->glyph->bitmap.width /*+ 2*/; // add the width of this glyph to our texture width
        // Note: We add 2 pixels of blank space between glyphs for padding - this helps reduce texture bleeding
        //       that can occur with antialiasing

        mHeight = std::max(mHeight, (u32)mFreeTypeFace->glyph->bitmap.rows);
    }

/*
    GLYPH METRICS (sample character: A)
    source: https://freetype.org/freetype2/docs/glyphs/glyphs-3.html


            .            xMin         xMax                       
            .              .           .                     
            .              .           .                     
            .              .   width   .                     
            .              .<.........>.                     
            .              .           .                     
            .              .           .                     
            .              *************....................................... yMax
            .  bearingX    *     X     *           ^                        ^
            ..............>*     XX    *           . bearingY               .
            .              *    X  X   *           .                        . height
            .              *   XXXXXX  *           .                        .
        ----O--------------*--X------X-*--------------------O---->          .
origin(0,0) .              * X        X*                    .               v
            .              *************....................................... yMin
            .                                               .
            .                                               .
            ...............................................>.
            .                 advance                       .
*/

    u32 texPos = 0;
    FOR_RANGE(c, 0, charSetCount)
    {
        _error = FT_Load_Char(mFreeTypeFace, c, FT_LOAD_DEFAULT);
        CHECK_MSG(!_error, "Failed to load Glyph: " + std::to_string(c));

        _error = FT_Render_Glyph(mFreeTypeFace->glyph, FT_RENDER_MODE_NORMAL);
        CHECK_MSG(!_error, "Failed to render Glyph: " + std::to_string(c));

        Vector2 bitmapSize = Vector2(mFreeTypeFace->glyph->bitmap.width, mFreeTypeFace->glyph->bitmap.rows);
        Vector2 glyphSizeInAtlasSpace = bitmapSize / Vector2(mWidth, mHeight);
        Vector2 textureOffset = Vector2((f32)texPos / (f32)mWidth, 0);

        FontGlyphMetricsData metrics;
        metrics.mSize = Vector2(mFreeTypeFace->glyph->metrics.width >> 6, mFreeTypeFace->glyph->metrics.height >> 6);
        metrics.mBoundingBoxMin = Vector2(mFreeTypeFace->bbox.xMin >> 6, mFreeTypeFace->bbox.yMin >> 6);
        metrics.mBoundingBoxMax = Vector2(mFreeTypeFace->bbox.xMax >> 6, mFreeTypeFace->bbox.yMax >> 6);
        metrics.mHoriBearing = Vector2(mFreeTypeFace->glyph->metrics.horiBearingX >> 6, mFreeTypeFace->glyph->metrics.horiBearingY >> 6);
        metrics.mVertBearing = Vector2(mFreeTypeFace->glyph->metrics.vertBearingX >> 6, mFreeTypeFace->glyph->metrics.vertBearingY >> 6);

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
            bitmapSize,
            Vector2(mFreeTypeFace->glyph->bitmap_left, mFreeTypeFace->glyph->bitmap_top),
            metrics,
            Rectangle(textureOffset, glyphSizeInAtlasSpace),
            nullptr
        };

        mGlyphs[c].mData = new byte[mFreeTypeFace->glyph->bitmap.width * mFreeTypeFace->glyph->bitmap.rows];

        std::memcpy(mGlyphs[c].mData, mFreeTypeFace->glyph->bitmap.buffer, mFreeTypeFace->glyph->bitmap.width * mFreeTypeFace->glyph->bitmap.rows);

        ImageUtils::flipImageVertically({mGlyphs[c].mData, mFreeTypeFace->glyph->bitmap.width, mFreeTypeFace->glyph->bitmap.rows}, 1);

        // Increase texture offset
        texPos += mGlyphs[c].mBitmapSize.x /*+ 2*/;
    }

    _error = FT_Done_Face(mFreeTypeFace);
    CHECK_MSG(!_error, "Failed to free font");
}

void FontData::freeGlyphsBuffers()
{
    FOR_RANGE(c, 0, MAX_GLYPHS)
    {
        mGlyphs[c].freeBuffer();
    }
}