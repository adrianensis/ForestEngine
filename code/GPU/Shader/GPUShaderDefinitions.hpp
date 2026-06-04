#pragma once

#include "GPU/Core/GPUDefinitions.h"
#include "GPU/Buffer/GPUUniformBuffer.hpp"

class GPUShaderDefinitions
{
public:

    class PrimitiveTypes
    {
    public:
        inline static const GPUDataType mVoid{"void",0, GPUPrimitiveDataType::VOID};
        inline static const GPUDataType mBool{"bool",sizeof(bool), GPUPrimitiveDataType::BOOL};
        inline static const GPUDataType mInt{"int",sizeof(GPU::i32), GPUPrimitiveDataType::INT};
        inline static const GPUDataType mUnsignedInt{"uint",sizeof(GPU::u32), GPUPrimitiveDataType::UNSIGNED_INT};
        inline static const GPUDataType mFloat{"float",sizeof(GPU::f32), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mVector2{"vec2",sizeof(Maths::Vector2), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mVector2i{"ivec2",sizeof(GPU::i32) * 2, GPUPrimitiveDataType::INT};
        inline static const GPUDataType mVector3{"vec3",sizeof(Maths::Vector3), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mVector3i{"ivec3",sizeof(GPU::i32) * 3, GPUPrimitiveDataType::INT};
        inline static const GPUDataType mFace = mVector3i;
        inline static const GPUDataType mVector4{"vec4",sizeof(Maths::Vector4), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mVector4i{"ivec4",sizeof(GPU::i32) * 4, GPUPrimitiveDataType::INT};
        inline static const GPUDataType mMatrix3{"mat3",sizeof(GPU::f32) * 3 * 3, GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mMatrix4{"mat4",sizeof(Maths::Matrix4), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mSampler2D{"sampler2D",0, GPUPrimitiveDataType::INT};
    };

    class Consts
    {
    public:
        inline static const GPUVariableDefinitionData mPI{{GPUStorage::CONST, PrimitiveTypes::mFloat, "PI"}, "3.14159265358979323846264338327950288"};
        inline static const GPUVariableDefinitionData mPI180{{GPUStorage::CONST, PrimitiveTypes::mFloat, "PI180"}, "PI/180.0"};
        inline static const GPUVariableDefinitionData mMaxBones{{GPUStorage::CONST, PrimitiveTypes::mInt, "MAX_BONES"}, std::to_string(GPUConstants::MAX_BONES)};
        inline static const GPUVariableDefinitionData mMaxBoneInfluence{{GPUStorage::CONST, PrimitiveTypes::mInt, "MAX_BONE_INFLUENCE"}, std::to_string(GPUConstants::MAX_BONE_INFLUENCE)};
    };

    class Uniforms
    {
    public:
        inline static const GPUVariableDefinitionData mSampler{GPUStorage::UNIFORM, PrimitiveTypes::mSampler2D, "uSampler"};
        static GPUVariableDefinitionData getSampler(const Core::HashedString& name)
        {
            GPUVariableDefinitionData namedSampler = mSampler;
            namedSampler.mName = Core::HashedString(namedSampler.mName.get() + "_" + name.get());
            return namedSampler;
        }
        inline static const GPUVariableDefinitionData mTextureHandle{GPUStorage::UNIFORM, /*PrimitiveTypes::mUnsignedInt*/PrimitiveTypes::mSampler2D, "textureHandle"};
        static GPUVariableDefinitionData getTextureHandle(const Core::HashedString& name)
        {
            GPUVariableDefinitionData namedTextureHandle = mTextureHandle;
            namedTextureHandle.mName = Core::HashedString(namedTextureHandle.mName.get() + "_" + name.get());
            return namedTextureHandle;
        }

        inline static const GPUVariableDefinitionData mTextures
        {
            {GPUStorage::UNIFORM, PrimitiveTypes::mSampler2D, "textures"}, "", " "
        };
    };

    class UniformBuffers
    {
    public:

        class GPUGlobalData
        {
        public:
            Maths::Matrix4 mProjectionViewMatrix;
            Maths::Vector4 mCameraPosition;
        };

        inline static const GPUUniformBufferData mGlobalData
        {
            GPUBufferType::UNIFORM,
            {
                {GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "projectionViewMatrix"},
                {GPUStorage::UNIFORM, PrimitiveTypes::mVector4, "cameraPosition"}
            },
            "GlobalData",
            "globalData",
            GPUDescriptorSetScope::LOCAL
        };

        // inline static const GPUUniformBufferData mTextures
        // {
        //     GPUBufferType::STORAGE,
        //     {
        //         {{GPUStorage::UNIFORM, PrimitiveTypes::mSampler2D, "textures"}, "", " "},
        //     },
        //     "Textures",
        //     "textures",
        //     GPUDescriptorSetScope::GLOBAL // TODO: this scope should be decided by user
        // };
        
        inline static const GPUUniformBufferData mModelMatrices
        {
            GPUBufferType::STORAGE,
            {
                {{GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "modelMatrices"}, "", " "},
            },
            "ModelMatrices",
            "modelMatrices",
            GPUDescriptorSetScope::LOCAL
        };

        inline static const GPUUniformBufferData mBonesMatrices
        {
            GPUBufferType::UNIFORM,
            {
                {{GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "bonesMatrices"}, "", std::to_string(GPUConstants::MAX_BONES)},
            },
            "BonesMatrices",
            "bonesMatrices",
            GPUDescriptorSetScope::LOCAL
        };
    };

    class VertexInput
    {
    public:
        class BoneVertexIDsData
        {
        public:
            GPU::i32 mBonesIDs[GPUConstants::MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};
        };

        class BoneVertexWeightsData
        {
        public:
            GPU::f32 mBonesWeights[GPUConstants::MAX_BONE_INFLUENCE] = {0.0f, 0.0f, 0.0f, 0.0f};
        };

        inline static const GPUVariableDefinitionData mPosition{GPUStorage::IN, PrimitiveTypes::mVector3, "in_position"};
        inline static const std::vector<GPUVariableDefinitionData> mTextureCoords
        {
            {GPUStorage::IN, PrimitiveTypes::mVector2, "in_texcoord0"},
            {GPUStorage::IN, PrimitiveTypes::mVector2, "in_texcoord1"},
            {GPUStorage::IN, PrimitiveTypes::mVector2, "in_texcoord2"},
            {GPUStorage::IN, PrimitiveTypes::mVector2, "in_texcoord3"}
        };
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::IN, PrimitiveTypes::mVector3, "in_normal"};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::IN, PrimitiveTypes::mVector4, "in_color"};
        inline static const GPUVariableDefinitionData mBonesIDs{GPUStorage::IN, PrimitiveTypes::mVector4i, "in_BoneIDs"};
        inline static const GPUVariableDefinitionData mBonesWeights{GPUStorage::IN, PrimitiveTypes::mVector4, "in_Weights"};
        inline static const GPUVariableDefinitionData mInstanceID{GPUStorage::IN, PrimitiveTypes::mUnsignedInt, "in_instanceID"};
        inline static const GPUVariableDefinitionData mObjectID{GPUStorage::IN, PrimitiveTypes::mUnsignedInt, "in_objectId"};
        inline static const GPUVariableDefinitionData mGPUShaderPropertiesInstanceID{GPUStorage::IN, PrimitiveTypes::mUnsignedInt, "in_shaderPropertiesInstanceId"};
    };

    class VertexOutput
    {
    public:
        inline static const GPUVariableDefinitionData mPosition{GPUStorage::OUT, PrimitiveTypes::mVector4, "gl_Position"};
        inline static const std::vector<GPUVariableDefinitionData> mTextureCoords
        {
            {GPUStorage::OUT, PrimitiveTypes::mVector2, "vTexcoord0"},
            {GPUStorage::OUT, PrimitiveTypes::mVector2, "vTexcoord1"},
            {GPUStorage::OUT, PrimitiveTypes::mVector2, "vTexcoord2"},
            {GPUStorage::OUT, PrimitiveTypes::mVector2, "vTexcoord3"}
        };
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::OUT, PrimitiveTypes::mVector3, "vNormal"};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::OUT, PrimitiveTypes::mVector4, "vColor"};
        inline static const GPUVariableDefinitionData mFragPosition{GPUStorage::OUT, PrimitiveTypes::mVector3, "vFragPosition"};
        inline static const GPUVariableDefinitionData mFragPositionLight{GPUStorage::OUT, PrimitiveTypes::mVector4, "vFragPositionLightSpace"};
        inline static const GPUVariableDefinitionData mInstanceID{GPUInterpolation::FLAT, GPUStorage::OUT, PrimitiveTypes::mUnsignedInt, "vInstanceID"};
        inline static const GPUVariableDefinitionData mObjectID{GPUInterpolation::FLAT, GPUStorage::OUT, PrimitiveTypes::mUnsignedInt, "vObjectId"};
        inline static const GPUVariableDefinitionData mGPUShaderPropertiesInstanceID{GPUInterpolation::FLAT, GPUStorage::OUT, PrimitiveTypes::mUnsignedInt, "vGPUShaderPropertiesInstanceId"};
    };

    class FragmentInput
    {
    public:
        inline static const std::vector<GPUVariableDefinitionData> mTextureCoords
        {
            {GPUStorage::IN, VertexOutput::mTextureCoords.at(0)},
            {GPUStorage::IN, VertexOutput::mTextureCoords.at(1)},
            {GPUStorage::IN, VertexOutput::mTextureCoords.at(2)},
            {GPUStorage::IN, VertexOutput::mTextureCoords.at(3)}
        };
        inline static const GPUVariableDefinitionData mColor{GPUStorage::IN, VertexOutput::mColor};
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::IN, VertexOutput::mNormal};
        inline static const GPUVariableDefinitionData mFragPosition{GPUStorage::IN, VertexOutput::mFragPosition};
        inline static const GPUVariableDefinitionData mFragPositionLight{GPUStorage::IN, VertexOutput::mFragPositionLight};
        inline static const GPUVariableDefinitionData mInstanceID{GPUInterpolation::FLAT, GPUStorage::IN, VertexOutput::mInstanceID};
        inline static const GPUVariableDefinitionData mObjectID{GPUInterpolation::FLAT, GPUStorage::IN, VertexOutput::mObjectID};
        inline static const GPUVariableDefinitionData mGPUShaderPropertiesInstanceID{GPUInterpolation::FLAT, GPUStorage::IN, VertexOutput::mGPUShaderPropertiesInstanceID};
    };

    class FragmentOutput
    {
    public:
        inline static const GPUVariableDefinitionData mColor{GPUStorage::OUT, PrimitiveTypes::mVector4, "FragColor"};
    };

    class Functions
    {
    public:
        inline static const GPUFunctionDefinition mMain
        {
            PrimitiveTypes::mVoid,
            "main"
        };

        inline static const GPUFunctionDefinition mCalculateBoneTransform
        {
            PrimitiveTypes::mMatrix4,
            "calculateBoneTransform"
        };
    };
};