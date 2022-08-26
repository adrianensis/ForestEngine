#include "Graphics/Material/Texture.hpp"
#include "Graphics/RenderContext.hpp"
#include "png.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::~Texture() 
{
	delete[] mData;
}

void Texture::init(const std::string& path)
{

	if (!mData)
	{
		mPath = path;
		loadImage();

		glGenTextures(1, &mTextureId);

		bind();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, getWidth(), getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, getData());


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_TEXTURE_MAG_FILTER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
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
