#include "Image/Font/Font.hpp"
#include "Image/ImageUtils.hpp"


NS_BEGIN(Font)
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

void FontData::loadFont(FontsLibrary& fontsLibrary, Core::HashedString fontFile, Core::u32 fontSize)
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

    // _error = FT_Set_Char_Size ( mFreeTypeFace, 0, fontSize * 64, 96, 96);
    _error = FT_Set_Pixel_Sizes ( mFreeTypeFace, 0, fontSize*2 /*scaled by 2 for better visualization*/);
    CHECK_MSG(!_error, "Failed to set pixel size");

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
    // Core::u32 fontSize = 18;
    // FT_Set_Char_Size( mFreeTypeFace, fontSize << 6, fontSize << 6, 96, 96);
    // if(_error)
    // {
    //     CHECK_MSG(false, "Failed to set char size");
    // }

    CHECK_MSG(!_error, "Failed to open font");

    Core::u32 charSetCount = MAX_GLYPHS;
    FOR_RANGE(c, 0, charSetCount)
    {
        _error = FT_Load_Char(mFreeTypeFace, c, FT_LOAD_DEFAULT);
        CHECK_MSG(!_error, "Failed to load Glyph: " + std::to_string(c));

        mWidth += mFreeTypeFace->glyph->bitmap.width /*+ 2*/; // add the width of this glyph to our texture width
        // Note: We add 2 pixels of blank space between glyphs for padding - this helps reduce texture bleeding
        //       that can occur with antialiasing

        mHeight = std::max(mHeight, (Core::u32)mFreeTypeFace->glyph->bitmap.rows);
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

    Core::u32 texPos = 0;
    FOR_RANGE(c, 0, charSetCount)
    {
        _error = FT_Load_Char(mFreeTypeFace, c, FT_LOAD_DEFAULT);
        CHECK_MSG(!_error, "Failed to load Glyph: " + std::to_string(c));

        _error = FT_Render_Glyph(mFreeTypeFace->glyph, FT_RENDER_MODE_NORMAL);
        CHECK_MSG(!_error, "Failed to render Glyph: " + std::to_string(c));

        Maths::Vector2 bitmapSize = Maths::Vector2(mFreeTypeFace->glyph->bitmap.width, mFreeTypeFace->glyph->bitmap.rows);
        Maths::Vector2 glyphSizeInAtlasSpace = bitmapSize / Maths::Vector2(mWidth, mHeight);
        Maths::Vector2 textureOffset = Maths::Vector2((Core::f32)texPos / (Core::f32)mWidth, 0);
        
        FontGlyphMetricsData metrics;
        metrics.mSize = Maths::Vector2(mFreeTypeFace->glyph->metrics.width >> 6, mFreeTypeFace->glyph->metrics.height >> 6);
        metrics.mBoundingBoxMin = Maths::Vector2(mFreeTypeFace->bbox.xMin >> 6, mFreeTypeFace->bbox.yMin >> 6);
        metrics.mBoundingBoxMax = Maths::Vector2(mFreeTypeFace->bbox.xMax >> 6, mFreeTypeFace->bbox.yMax >> 6);
        metrics.mHoriBearing = Maths::Vector2(mFreeTypeFace->glyph->metrics.horiBearingX >> 6, mFreeTypeFace->glyph->metrics.horiBearingY >> 6);
        metrics.mVertBearing = Maths::Vector2(mFreeTypeFace->glyph->metrics.vertBearingX >> 6, mFreeTypeFace->glyph->metrics.vertBearingY >> 6);
        
        mMaxAscender = std::max(metrics.mHoriBearing.y, mMaxAscender);
        mMaxDescender = std::max(metrics.mSize.y - metrics.mHoriBearing.y, mMaxDescender);

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
            Maths::Vector2(mFreeTypeFace->glyph->advance.x >> 6, mFreeTypeFace->glyph->advance.y >> 6),
            bitmapSize,
            Maths::Vector2(mFreeTypeFace->glyph->bitmap_left, mFreeTypeFace->glyph->bitmap_top),
            metrics,
            Maths::Cube(textureOffset, glyphSizeInAtlasSpace),
            nullptr
        };

        mGlyphs[c].mData = new Core::byte[mFreeTypeFace->glyph->bitmap.width * mFreeTypeFace->glyph->bitmap.rows];

        std::memcpy(mGlyphs[c].mData, mFreeTypeFace->glyph->bitmap.buffer, mFreeTypeFace->glyph->bitmap.width * mFreeTypeFace->glyph->bitmap.rows);

        // ImageUtils::flipImageVertically({mGlyphs[c].mData, mFreeTypeFace->glyph->bitmap.width, mFreeTypeFace->glyph->bitmap.rows}, 1);

        // Increase texture offset
        Core::u32 width = mGlyphs[c].mBitmapSize.x;
        if(width == 0)
        {
            width = mGlyphs[c].mAdvance.x;
        }
        texPos += width /*+ 2*/;
    }

    mGlyphAtlasData = new Core::byte[mWidth * mHeight * 1 /*1 channel*/];

    Core::u32 bytesOffset = 0;
    FOR_RANGE(c, 0, charSetCount)
    {
        Core::u32 width = mGlyphs[c].mBitmapSize.x;
        Core::u32 height = mGlyphs[c].mBitmapSize.y;

        // " " space case! no size, no data, not supported by vulkan
        if(width == 0)
        {
            bytesOffset += mGlyphs[c].mAdvance.x;
        }
        else
        {
            FOR_RANGE(i, 0, height)
            {
                FOR_RANGE(j, 0, width)
                {
                    mGlyphAtlasData[(i* (mWidth)) + j + bytesOffset] = mGlyphs[c].mData[(i*width) + j];
                }
            }
            
            bytesOffset += width;
        }
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

    delete[] mGlyphAtlasData;
}

NS_END