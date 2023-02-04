#pragma once

#include "Core/Module.hpp"


class Texture: public ObjectBase
{
    GENERATE_METADATA(Texture)
	DECLARE_SERIALIZATION()

public:
    ~Texture() override;

    void init(const std::string& path);
    void bind() const;

private:
    void loadImage();
	void deleteData();

private:
	u32 mTextureId = 0;
	byte* mData = nullptr;
	u32 mWidth = 0;
	u32 mHeight = 0;
	std::string mPath;
};
