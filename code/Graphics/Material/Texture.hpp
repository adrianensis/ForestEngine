#pragma once

#include "Core/Module.hpp"
class Texture: public ObjectBase
{
    GENERATE_METADATA(Texture)
	PRI u32 mTextureId = -1;
	PRI byte* mData = nullptr;
	PRI u32 mWidth = 0;
	PRI u32 mHeight = 0;
	PRI std::string mPath;

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
	GET_RC(Path)
};