#include "Graphics/Material/Texture.hpp"
#include "Graphics/RenderContext.hpp"
#include "png.h"


Texture::~Texture() 
{
	delete[] mData;
}

void Texture::init(const std::string& path)
{

	if (!mData)
	{
		mPath = path;
		mData = readPNG();

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

void Texture::serialize(JSON& json) const 
{
}

void Texture::deserialize(const JSON& json) 
{
}

byte* Texture::readPNG()
{
	png_byte header[8];

	FILE *fp = fopen(mPath.c_str(), "rb");
	if (!fp)
	{
		ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
	}

	fread(header, 1, 8, fp);

	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png)
	{
		fclose(fp);
		ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
												NULL, NULL);
	if (!png_ptr)
	{
		fclose(fp);
		ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
	}

	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
	}

	png_init_io(png_ptr, fp);

	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
				NULL, NULL, NULL);

	mWidth = (u32)twidth;
	mHeight = (u32)theight;

	png_read_update_info(png_ptr, info_ptr);

	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	png_byte *image_data = new (png_byte[rowbytes* mHeight]);
	if (!image_data)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
	}

	png_bytep *row_pointers = new (png_bytep[mHeight]);
	if (!row_pointers)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		fclose(fp);
		ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
	}
	for (u32 i = 0; i < mHeight; ++i)
		row_pointers[mHeight - 1 - i] = image_data + i * rowbytes;

	png_read_image(png_ptr, row_pointers);

	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] row_pointers;
	fclose(fp);

	return image_data;
}
