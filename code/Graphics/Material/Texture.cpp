#include "Graphics/Material/Texture.hpp"
#include "Graphics/RenderContext.hpp"
#include "png.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::~Texture() 
{
	deleteData();
	GET_SYSTEM(RenderContext).deleteTexture(mTextureId);
}

void Texture::init(const std::string& path)
{
	if (!mData)
	{
		mPath = path;
		loadImage();
        mTextureId = GET_SYSTEM(RenderContext).createTexture(mWidth, mHeight, mData);
		deleteData();
	}
}

void Texture::bind() const
{
    GET_SYSTEM(RenderContext).enableTexture(mTextureId);
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
	//stbi_set_flip_vertically_on_load(true);

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

void Texture::deleteData()
{
	if(mData)
	{
		delete[] mData;
		mData = nullptr;
	}
}