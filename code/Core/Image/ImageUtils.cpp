#include "Core/Image/ImageUtils.hpp"

#include "png.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImageData ImageUtils::loadImage(HashedString path)
{
	i32 width, height, original_number_channels;
	stbi_set_flip_vertically_on_load(true);

	// if(path.find("\\") != std::string::npos)
	// {
	// 	path.replace(path.find("\\"), 1, "/");
	// }

    ImageData imageData;
	imageData.mData = stbi_load(path.get().c_str(), &width, &height, &original_number_channels, STBI_rgb_alpha);
	imageData.mWidth = width;
    imageData.mHeight = height;
    return imageData;
}

void ImageUtils::freeImage(const ImageData& imageData)
{
    stbi_image_free(imageData.mData);
}

void ImageUtils::flipImageVertically(const ImageData& imageData, u32 bytes_per_pixel)
{
    stbi__vertical_flip(imageData.mData, imageData.mWidth, imageData.mHeight, bytes_per_pixel);
}
