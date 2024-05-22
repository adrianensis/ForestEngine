#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"

class GPUBuiltIn
{
public:

    inline static const u32 MAX_BONE_INFLUENCE = 4;
    inline static const u32 MAX_BONES = 65;

    class PrimitiveTypes
    {
    public:
        inline static const GPUDataType mVoid{"void",0, GPUPrimitiveDataType::VOID};
        inline static const GPUDataType mBool{"bool",sizeof(bool), GPUPrimitiveDataType::BOOL};
        inline static const GPUDataType mInt{"int",sizeof(i32), GPUPrimitiveDataType::INT};
        inline static const GPUDataType mUnsignedInt{"uint",sizeof(u32), GPUPrimitiveDataType::INT};
        inline static const GPUDataType mFloat{"float",sizeof(f32), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mVector2{"vec2",sizeof(Vector2), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mVector2i{"ivec2",sizeof(i32) * 2, GPUPrimitiveDataType::INT};
        inline static const GPUDataType mVector3{"vec3",sizeof(Vector3), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mVector3i{"ivec3",sizeof(i32) * 3, GPUPrimitiveDataType::INT};
        inline static const GPUDataType mFace = mVector3i;
        inline static const GPUDataType mVector4{"vec4",sizeof(Vector4), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mVector4i{"ivec4",sizeof(i32) * 4, GPUPrimitiveDataType::INT};
        inline static const GPUDataType mMatrix3{"mat3",sizeof(f32) * 3 * 3, GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mMatrix4{"mat4",sizeof(Matrix4), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mSampler2D{"sampler2D",0, GPUPrimitiveDataType::INT};
    };

    class Consts
    {
    public:
        // TODO: unsigned int?
        inline static const GPUVariableDefinitionData mPI{{GPUStorage::CONST, PrimitiveTypes::mFloat, "PI"}, "3.14159265358979323846264338327950288"};
        inline static const GPUVariableDefinitionData mPI180{{GPUStorage::CONST, PrimitiveTypes::mFloat, "PI180"}, "PI/180.0"};
        inline static const GPUVariableDefinitionData mMaxBones{{GPUStorage::CONST, PrimitiveTypes::mInt, "MAX_BONES"}, std::to_string(MAX_BONES)};
        inline static const GPUVariableDefinitionData mMaxBoneInfluence{{GPUStorage::CONST, PrimitiveTypes::mInt, "MAX_BONE_INFLUENCE"}, std::to_string(MAX_BONE_INFLUENCE)};
    };

    class Uniforms
    {
    public:
        inline static const GPUVariableDefinitionData mSampler{GPUStorage::UNIFORM, PrimitiveTypes::mSampler2D, "uSampler"};
        static GPUVariableDefinitionData getSamplerName(const ConstString& name)
        {
            GPUVariableDefinitionData numberedSampler = mSampler;
            numberedSampler.mName = ConstString(numberedSampler.mName.get() + "_" + name.get());
            return numberedSampler;
        }
    };

    class SharedBuffers
    {
    public:

        class GPUGlobalData
        {
        public:
            Matrix4 mProjectionViewMatrix;
            alignas(16) Vector3 mCameraPosition;
        };

        inline static const GPUSharedBufferData mGlobalData
        {
            GPUBufferType::UNIFORM,
            {
                {GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "projectionViewMatrix"},
                {GPUStorage::UNIFORM, PrimitiveTypes::mVector3, "cameraPosition"}
            },
            "GlobalData",
            "globalData"
        };
        
        inline static const GPUSharedBufferData mModelMatrices
        {
            GPUBufferType::STORAGE,
            {
                {{GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "modelMatrices"}, "", " "},
            },
            "ModelMatrices",
            "modelMatrices"
        };

        inline static const GPUSharedBufferData mBonesMatrices
        {
            GPUBufferType::UNIFORM,
            {
                {{GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "bonesMatrices"}, "", std::to_string(MAX_BONES)},
            },
            "BonesMatrices",
            "bonesMatrices"
        };
    };

    class VertexInput
    {
    public:
        inline static const GPUVariableDefinitionData mPosition{GPUStorage::IN, PrimitiveTypes::mVector3, "in_position"};
        inline static const GPUVariableDefinitionData mTextureCoord{GPUStorage::IN, PrimitiveTypes::mVector2, "in_texcoord"};
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::IN, PrimitiveTypes::mVector3, "in_normal"};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::IN, PrimitiveTypes::mVector4, "in_color"};
        inline static const GPUVariableDefinitionData mBonesIDs{GPUStorage::IN, PrimitiveTypes::mVector4i, "in_BoneIDs"};
        inline static const GPUVariableDefinitionData mBonesWeights{GPUStorage::IN, PrimitiveTypes::mVector4, "in_Weights"};
        inline static const GPUVariableDefinitionData mInstanceID{GPUStorage::IN, PrimitiveTypes::mUnsignedInt, "in_instanceID"};
        inline static const GPUVariableDefinitionData mObjectID{GPUStorage::IN, PrimitiveTypes::mUnsignedInt, "in_objectId"};
        inline static const GPUVariableDefinitionData mMaterialInstanceID{GPUStorage::IN, PrimitiveTypes::mUnsignedInt, "in_materialInstanceId"};
    };

    class VertexOutput
    {
    public:
        inline static const GPUVariableDefinitionData mPosition{GPUStorage::OUT, PrimitiveTypes::mVector4, "gl_Position"};
        inline static const GPUVariableDefinitionData mTextureCoord{GPUStorage::OUT, PrimitiveTypes::mVector2, "vTexcoord"};
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::OUT, PrimitiveTypes::mVector3, "vNormal"};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::OUT, PrimitiveTypes::mVector4, "vColor"};
        inline static const GPUVariableDefinitionData mFragPosition{GPUStorage::OUT, PrimitiveTypes::mVector3, "vFragPosition"};
        inline static const GPUVariableDefinitionData mFragPositionLight{GPUStorage::OUT, PrimitiveTypes::mVector4, "vFragPositionLightSpace"};
        inline static const GPUVariableDefinitionData mInstanceID{GPUInterpolation::FLAT, GPUStorage::OUT, PrimitiveTypes::mUnsignedInt, "vInstanceID"};
        inline static const GPUVariableDefinitionData mObjectID{GPUInterpolation::FLAT, GPUStorage::OUT, PrimitiveTypes::mUnsignedInt, "vObjectId"};
        inline static const GPUVariableDefinitionData mMaterialInstanceID{GPUInterpolation::FLAT, GPUStorage::OUT, PrimitiveTypes::mUnsignedInt, "vMaterialInstanceId"};
    };

    class FragmentInput
    {
    public:
        inline static const GPUVariableDefinitionData mTextureCoord{GPUStorage::IN, VertexOutput::mTextureCoord};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::IN, VertexOutput::mColor};
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::IN, VertexOutput::mNormal};
        inline static const GPUVariableDefinitionData mFragPosition{GPUStorage::IN, VertexOutput::mFragPosition};
        inline static const GPUVariableDefinitionData mFragPositionLight{GPUStorage::IN, VertexOutput::mFragPositionLight};
        inline static const GPUVariableDefinitionData mInstanceID{GPUInterpolation::FLAT, GPUStorage::IN, VertexOutput::mInstanceID};
        inline static const GPUVariableDefinitionData mObjectID{GPUInterpolation::FLAT, GPUStorage::IN, VertexOutput::mObjectID};
        inline static const GPUVariableDefinitionData mMaterialInstanceID{GPUInterpolation::FLAT, GPUStorage::IN, VertexOutput::mMaterialInstanceID};
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