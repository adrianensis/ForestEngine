#pragma once

#include "Core/Core.hpp"
#include "Core/Memory/SlotsManager.hpp"
#include "GPU/Texture/GPUTexture.hpp"

class GPUTextureManager
{
public:
    void init();
    void terminate();
    Core::WeakPtr<GPUTexture> loadTexture(GPUContext* gpuContext, const GPUTextureData& gpuTextureData);

private:
    std::vector<Core::OwnerPtr<GPUTexture>> mTextures;
    Core::SlotsManager mTextureSlotManager;
    std::unordered_map<Core::HashedString, Core::Slot> mTexturesByPath;
    inline static const Core::u32 mInitialTextures = 300;
};

