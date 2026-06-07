#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Window/GPUWindow.hpp"
#include "GPU/Descriptors/GPUDescriptorManager.hpp"
class GPUSkeletalAnimationManager;
class GPUShaderManager;
class GPUTextureManager;

class GPUInstance
{
public:
    void init(IGPUWindow* gpuWindow);
    void terminate();

	static GPUInstance* getInstancePtr()
	{
		if (!mInstance)
		{
			mInstance = new GPUInstance();
		}

		return mInstance;
	}

    static GPUInstance& getInstance()
	{
		return *getInstancePtr();
	}

	static void deleteInstance()
	{
		if (mInstance)
		{
			GPUInstance* ptr = mInstance;
			mInstance = nullptr;
			delete ptr;
		}
	}

public:
    GPUContext* mGPUContext = nullptr;
    GPUTextureManager* mGPUTextureManager = nullptr;
    GPUShaderManager* mGPUShaderManager = nullptr;
    GPUSkeletalAnimationManager* mGPUSkeletalAnimationManager = nullptr;
    GPUDescriptorManager mGPUDescriptorManager;

	inline static GPUInstance* mInstance = nullptr;
};