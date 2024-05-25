#pragma once

#include "Core/StdCore.hpp"
#include "Core/ConstString/ConstString.hpp"

class ImageData
{
public:
    byte* mData;
    u32 mWidth = 0;
	u32 mHeight = 0;
};

class ImageUtils
{
public:
	static ImageData loadImage(ConstString path);
	static void freeImage(const ImageData& imageData);
	static void flipImageVertically(const ImageData& imageData, u32 bytes_per_pixel);
};
