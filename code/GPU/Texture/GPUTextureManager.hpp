#pragma once

#include "Core/Core.hpp"
#include "Core/Memory/SlotsManager.hpp"
#include "GPU/Texture/GPUTexture.hpp"

class GPUTextureManager
{
public:
    void init(GPUContext* gpuContext);
    void terminate();
    GPUTextureHandle loadTexture(const GPUTextureData& gpuTextureData);
    const GPUTexture& getTexture(const GPUTextureHandle& handle) const;

private:
    GPUContext* mGPUContext = nullptr;
    std::vector<GPUTexture> mTextures;
    Core::SlotsManager mTextureSlotManager;
    std::unordered_map<Core::HashedString, Core::Slot> mTexturesByPath;
    inline static const Core::u32 mInitialTextures = 300;
};

