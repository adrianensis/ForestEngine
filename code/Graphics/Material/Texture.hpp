#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Core/Module.hpp"


class Texture: public ObjectBase
{
    GENERATE_METADATA(Texture)

public:
    ~Texture() override;

    void init(const std::string& path);
    void bind();
    void serialize(JSON& json) const override;
    void deserialize(const JSON& json) override;

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