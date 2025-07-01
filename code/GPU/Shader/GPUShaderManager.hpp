#pragma once

#include "Core/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPUShaderManager: public System::System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    Core::WeakPtr<GPUTexture> loadTexture(const GPUTextureData& gpuTextureData);

    template<class T, class P> T_EXTENDS(T, GPUShader)
    Core::WeakPtr<GPUShader> createShader(const GPUShaderData& shaderData, const P& propertiesBlockDefault)
    {
        Core::WeakPtr<GPUShader> shader = mShaders.emplace_back(Core::OwnerPtr<GPUShader>::moveCast(Core::OwnerPtr<T>::newObject()));
        Core::GenericObjectBuffer propertiesBlockDefaultBuffer;
        propertiesBlockDefaultBuffer.set<P>();
        propertiesBlockDefaultBuffer.get<P>() = propertiesBlockDefault;
        shader->init(shaderData, propertiesBlockDefaultBuffer, mShaders.size() - 1);
        postGPUShaderCreated(shader);

        return shader;
    }

    Core::WeakPtr<GPUShaderPropertiesInstance> createGPUShaderPropertiesInstance(Core::WeakPtr<GPUShader> shader);
    void freeGPUShaderPropertiesInstance(Core::WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance);

    void setGPUShaderPropertiesInstanceProperties(const Core::WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance);
    void setGPUShaderPropertiesInstanceDirty(Core::u32 id);

    const GPUUniformBuffer& getGPUShaderPropertiesGPUUniformBuffer(Core::WeakPtr<GPUShader> shader) const;
    Core::Slot requestGPUShaderPropertiesInstanceSlot(Core::WeakPtr<GPUShader> shader);

    const std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>>& getGPUShaderTextureBindings(Core::u32 id) const;
    
private:
    void postGPUShaderCreated(Core::WeakPtr<GPUShader> shader);
    void loadGPUShaderTextures(Core::WeakPtr<GPUShader> shader);
    void initGPUShaderPropertiesInstancePropertiesUniformBuffer(Core::WeakPtr<GPUShader> shader);

    class GPUShaderPropertyBlockRenderState
    {
    public:
        Core::ByteBuffer mGPUShaderPropertiesBlockArray;
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
        Core::SlotsManager mSlotsManager;
    };

	std::unordered_map<Core::ClassId, GPUShaderPropertyBlockRenderState> mGPUShaderPropertyBlockRenderStates;
	
    std::vector<Core::OwnerPtr<GPUTexture>> mTextures;
    std::vector<TextureHandle> mTextureHandles;
    std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>> mTexturesByPath;
	std::unordered_map<Core::u32, std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>>> mTextureBindingsByShader;
    std::vector<Core::OwnerPtr<GPUShader>> mShaders;
    std::vector<Core::OwnerPtr<GPUShaderPropertiesInstance>> mGPUShaderPropertiesInstances;
    std::unordered_set<Core::u32> mDirtyGPUShaderPropertiesInstances;
    inline static const Core::u32 mInitialInstances = 20000;
    inline static const Core::u32 mInitialTextures = 300;
};
REGISTER_CLASS(GPUShaderManager);
