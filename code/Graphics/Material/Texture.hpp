#pragma once

#include "Core/Minimal.hpp"
#include "Core/Font/Font.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

class TextureData
{
public:
    HashedString mPath;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
    bool mIsFont = false;
    FontData mFontData;
};

class Texture: public ObjectBase
{
public:
    void init(const TextureData& textureData, u32 id);
    void terminate();
    virtual void onPoolFree() override { terminate(); };
    void enable(u32 textureUnit) const;
    void disable(u32 textureUnit) const;

private:
	u32 mGPUTextureId = 0;
    TextureHandle mGPUTextureHandle = 0;
	u32 mWidth = 0;
	u32 mHeight = 0;
	u32 mID = 0;
	TextureData mTextureData;

public:
    GET(ID)
    GET(GPUTextureHandle)
};
REGISTER_CLASS(Texture);
