#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Material/Texture.hpp"
#include "Graphics/RenderContext.hpp"
//#include "SOIL/SOIL.h"
#include "png.h"
#endif

class Texture: public ObjectBase
{
    GENERATE_METADATA(Texture)

public:

	CPP ~Texture() override
	{
		delete[] mData;
	}

	CPP void init(const std::string& path)
	{
		//TRACE()

		if (!mData)
		{
			mPath = path;
			mData = readPNG();

			glGenTextures(1, &mTextureId);

			bind();

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, getWidth(), getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, getData());

			//glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_TEXTURE_MAG_FILTER
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
	}

	CPP void bind()
	{
		//TRACE()
		glBindTexture(GL_TEXTURE_2D, mTextureId);
	}

	CPP void serialize(JSON& json) const override
	{
		//DO_SERIALIZE("path", mPath)
	}

	CPP void deserialize(const JSON& json) override
	{
		//mPath = json["path"];
	}

private:
	CPP byte* readPNG()
	{
		//header for testing if it is a png
		png_byte header[8];

		//open file as binary
		FILE *fp = fopen(mPath.c_str(), "rb");
		if (!fp)
		{
			ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
		}

		//read the header
		fread(header, 1, 8, fp);

		//test if png
		int is_png = !png_sig_cmp(header, 0, 8);
		if (!is_png)
		{
			fclose(fp);
			ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
		}

		//create png struct
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
													NULL, NULL);
		if (!png_ptr)
		{
			fclose(fp);
			ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
		}

		//create png info struct
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			fclose(fp);
			ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
		}

		//create png info struct
		png_infop end_info = png_create_info_struct(png_ptr);
		if (!end_info)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			fclose(fp);
			ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
		}

		//png error stuff, not sure libpng man suggests this.
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			fclose(fp);
			ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
		}

		//init png reading
		png_init_io(png_ptr, fp);

		//let libpng know you already read the first 8 bytes
		png_set_sig_bytes(png_ptr, 8);

		// read all the info up to the image data
		png_read_info(png_ptr, info_ptr);

		//variables to pass to get info
		int bit_depth, color_type;
		png_uint_32 twidth, theight;

		// get info about png
		png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
					NULL, NULL, NULL);

		//update width and height based on png info
		mWidth = (u32)twidth;
		mHeight = (u32)theight;

		// Update the png info struct.
		png_read_update_info(png_ptr, info_ptr);

		// Row size in bytes.
		int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

		// Allocate the image_data as a big block, to be given to opengl
		png_byte *image_data = new (png_byte[rowbytes* mHeight]);
		if (!image_data)
		{
			//clean up memory and close stuff
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			fclose(fp);
			ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
		}

		//row_pointers is for pointing to image_data for reading the png with libpng
		png_bytep *row_pointers = new (png_bytep[mHeight]);
		if (!row_pointers)
		{
			//clean up memory and close stuff
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			delete[] image_data;
			fclose(fp);
			//return TEXTURE_LOAD_ERROR;
			ASSERT_MSG(false, "Texture load error"); // TODO : more descriptive
		}
		// set the individual row_pointers to point at the correct offsets of image_data
		for (u32 i = 0; i < mHeight; ++i)
			row_pointers[mHeight - 1 - i] = image_data + i * rowbytes;

		//read the png into image_data through row_pointers
		png_read_image(png_ptr, row_pointers);

		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		//delete[] image_data;
		delete[] row_pointers;
		fclose(fp);

		return image_data;
	}

private: 
	u32 mTextureId = -1;
	byte* mData = nullptr;
	u32 mWidth = 0;
	u32 mHeight = 0;
	std::string mPath;

public:
	GET(TextureId)
	GET(Data)
	GET(Width)
	GET(Height)
	CRGET(Path)
};