#pragma once

#include "Core/Module.hpp"
class Texture: public ObjectBase
{
    GENERATE_METADATA(Texture)
private: 
	u32 mTextureId = -1;
	byte* mData = nullptr;
	u32 mWidth = 0;
	u32 mHeight = 0;
	std::string mPath;

	byte *readPNG();

public:
	~Texture() override;

	void init(const std::string& path);
	void bind();

	void serialize(JSON& json) const override
	{
		//DO_SERIALIZE("path", mPath)
	}

	void deserialize(const JSON& json) override
	{
		//mPath = json["path"];
	}

	GET(TextureId)
	GET(Data)
	GET(Width)
	GET(Height)
	CRGET(Path)
};