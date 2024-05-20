#include "Graphics/Material/MaterialRuntime/MaterialRuntime.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"

void MaterialRuntime::init(PoolHandler<Material> material)
{
    mMaterialRuntimeData.mMaterial = material;

    GPUStructDefinition propertiesBlockStructDefinition =
    {
        "propertiesBlockStruct",
        {
            generateMaterialPropertiesBlock()
        }
    };

    GPUDataType propertiesBlockStructDataType =
    {
        propertiesBlockStructDefinition.mName,
        propertiesBlockStructDefinition.getTypeSizeInBytes(),
        GPUPrimitiveDataType::STRUCT
    };
    
    GPUSharedBufferData propertiesBlockSharedBufferData =
    {
        GPUBufferType::STORAGE,
        {
            {{GPUStorage::UNIFORM, propertiesBlockStructDataType, "propertiesBlockArray"}, "", " "}
        },
        "PropertiesBlock",
        "propertiesBlock"
    };

    mMaterialRuntimeData.mPropertiesBlockStructDefinition = propertiesBlockStructDefinition;
    mMaterialRuntimeData.mPropertiesBlockSharedBufferData = propertiesBlockSharedBufferData;
}

void MaterialRuntime::initFromCopy(Ptr<const MaterialRuntime> materialRuntime)
{
    mMaterialRuntimeData = materialRuntime->mMaterialRuntimeData;
}

std::vector<GPUStructDefinition::GPUStructVariable> MaterialRuntime::generateMaterialPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {GPUBuiltIn::PrimitiveTypes::mInt, "_emptyStructFixHack"}
    };

    return propertiesBlock;
}