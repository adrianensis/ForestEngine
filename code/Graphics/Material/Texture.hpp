#pragma once

#include "Core/Module.hpp"


class Texture: public ObjectBase
{
    GENERATE_METADATA(Texture)
	DECLARE_SERIALIZATION()

public:
    ~Texture() override;

    void init(const std::string& path);
    void bind();

private:
    void loadImage();
	void deleteData();

public: 
	u32 mTextureId = -1;
	byte* mData = nullptr;
	u32 mWidth = 0;
	u32 mHeight = 0;
	std::string mPath;
};
