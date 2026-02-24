#pragma once

#include "Core/Core.hpp"
#include "GPU/Image/GPUTexture.hpp"

class GPUTextureManager
{
public:
    void init();
    void terminate();
    Core::WeakPtr<GPUTexture> loadTexture(GPUContext* gpuContext, const GPUTextureData& gpuTextureData);

private:
    std::vector<Core::OwnerPtr<GPUTexture>> mTextures;
    std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>> mTexturesByPath;
    inline static const Core::u32 mInitialTextures = 300;
};

