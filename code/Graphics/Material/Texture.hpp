#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Core/Module.hpp"


class Texture: public ObjectBase
{
    GENERATE_METADATA(Texture)
	DECLARE_SERIALIZATION()

public:
    ~Texture() override;

    void init(CR(std::string) path);
    void bind();

private:
    byte* readPNG();

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

#endif