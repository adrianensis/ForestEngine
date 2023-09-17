#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "png.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::~Texture() 
{
    GET_SYSTEM(GPUInterface).deleteTexture(mTextureId);
}

void Texture::init(const std::string& path, bool createMipMap)
{
	if (!mData)
	{
		mPath = path;
		loadImage();
        mTextureId = GET_SYSTEM(GPUInterface).createTexture(mWidth, mHeight, GL_RGBA, mData, createMipMap);
		stbi_image_free(mData);
		mData = nullptr;
	}
}

void Texture::bind() const
{
    GET_SYSTEM(GPUInterface).enableTexture(mTextureId);
}

IMPLEMENT_SERIALIZATION(Texture)
{
}

IMPLEMENT_DESERIALIZATION(Texture)
{
}

void Texture::loadImage()
{
	i32 width, height, original_number_channels;
	stbi_set_flip_vertically_on_load(true);

	if(mPath.find("\\") != std::string::npos)
	{
		mPath.replace(mPath.find("\\"), 1, "/");
	}

	mData = stbi_load(mPath.c_str(), &width, &height, &original_number_channels, STBI_rgb_alpha);

	// save info only if data is ok
	if(mData)
	{
		mWidth = width;
		mHeight = height;
	}
}