#include "GPU/Descriptors/GPUDescriptorLayout.hpp"

void GPUDescriptorLayout::init(const GPUDescriptorLayoutData& gpuDescriptorLayoutData, GPUContext* gpuContext)
{
    mGPUContext = gpuContext;
    mGPUDescriptorLayoutData = gpuDescriptorLayoutData;

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    if(gpuDescriptorLayoutData.mIsBindless)
    {
        VkDescriptorSetLayoutBinding globalTextureBinding{};
        globalTextureBinding.binding = gpuDescriptorLayoutData.mBindlessTexturesArrayBinding;
        globalTextureBinding.descriptorCount = gpuDescriptorLayoutData.mMaxBindlessTextures;
        globalTextureBinding.stageFlags = VK_SHADER_STAGE_ALL;
        globalTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

        bindings.push_back(globalTextureBinding);
    }
    else
    {
        Core::u32 samplersBindingIndexOffset = mGPUDescriptorLayoutData.mUniformBuffers.size();
        FOR_ARRAY(i, mGPUDescriptorLayoutData.mUniformBuffers)
        {
            const GPUUniformBuffer& uniformBuffer = mGPUDescriptorLayoutData.mUniformBuffers[i];
    
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = i;
            layoutBinding.descriptorCount = 1;
            layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            switch (uniformBuffer.getGPUUniformBufferData().mType)
            {
            case GPUBufferType::UNIFORM:
                layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case GPUBufferType::STORAGE:
                layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            default:
                CHECK_MSG(false, "Not supported buffer type!")
            }
    
            bindings.push_back(layoutBinding);
        }
    
        FOR_ARRAY(i, mGPUDescriptorLayoutData.mTextureBindings)
        {
            // const GPUTextureBinding& textureBinding = mGPUDescriptorLayoutData.mTextureBindings[i];
    
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = i + samplersBindingIndexOffset;
            layoutBinding.descriptorCount = 1;
            layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    
            layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            layoutBinding.pImmutableSamplers = nullptr;
    
            bindings.push_back(layoutBinding);
        }
    }
    
    VkDescriptorBindingFlags bindingFlags = 
    VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | 
    VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

    if(gpuDescriptorLayoutData.mIsBindless)
    {
        bindingFlags |= 
        VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
    }

    std::vector<VkDescriptorBindingFlags> allBindingFlags(bindings.size(), bindingFlags);

    VkDescriptorSetLayoutBindingFlagsCreateInfo vkDescriptorSetLayoutBindingFlagsCreateInfo{};
    vkDescriptorSetLayoutBindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    vkDescriptorSetLayoutBindingFlagsCreateInfo.pBindingFlags = allBindingFlags.data();
    vkDescriptorSetLayoutBindingFlagsCreateInfo.pNext = nullptr;
    vkDescriptorSetLayoutBindingFlagsCreateInfo.bindingCount = static_cast<Core::u32>(bindings.size());
    
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<Core::u32>(bindings.size());
    layoutInfo.pBindings = bindings.data();
    layoutInfo.pNext = &vkDescriptorSetLayoutBindingFlagsCreateInfo;
    layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    constexpr VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    if (vkCreateDescriptorSetLayout(mGPUContext->vulkanDevice->getDevice(), &layoutInfo, allocationCallbacks, &descriptorSetLayout) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not create descrptor set layout");
    }
}

void GPUDescriptorLayout::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyDescriptorSetLayout(mGPUContext->vulkanDevice->getDevice(), descriptorSetLayout, allocationCallbacks);
}