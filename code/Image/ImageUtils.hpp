#pragma once

#include "Core/StdCore.hpp"
#include "Core/HashedString/HashedString.hpp"
#include "Core/StdMacros.hpp"

NS_BEGIN(Image)
class ImageData
{
public:
    Core::byte* mData = nullptr;
    Core::u32 mWidth = 0;
	Core::u32 mHeight = 0;
};

class ImageUtils
{
public:
	static ImageData loadImage(Core::HashedString path);
	static void freeImage(const ImageData& imageData);
	static void flipImageVertically(const ImageData& imageData, Core::u32 bytes_per_pixel);
};
NS_END
