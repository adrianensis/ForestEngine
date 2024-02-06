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
        inline static const GPUDataType mMatrix4{"mat4",sizeof(Matrix4), GPUPrimitiveDataType::FLOAT};
        inline static const GPUDataType mSampler2D{"sampler2D",0, GPUPrimitiveDataType::INT};
    };

    class StructDefinitions
    {
    public:
        inline static const GPUStructDefinition mLight
        {
            "light",
            {
                {PrimitiveTypes::mVector3, "position"},
                {PrimitiveTypes::mVector3, "color"},
                {PrimitiveTypes::mFloat, "ambientIntensity"},
                {PrimitiveTypes::mFloat, "specularIntensity"}
            }
        };
    };

    class StructTypes
    {
    public:
        inline static const GPUDataType mLight{StructDefinitions::mLight.mName, StructDefinitions::mLight.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    };

    class Consts
    {
    public:
        // TODO: unsigned int?
        inline static const GPUVariableDefinitionData mMaxBones{{GPUStorage::CONST, PrimitiveTypes::mInt, "MAX_BONES"}, std::to_string(MAX_BONES)};
        inline static const GPUVariableDefinitionData mMaxBoneInfluence{{GPUStorage::CONST, PrimitiveTypes::mInt, "MAX_BONE_INFLUENCE"}, std::to_string(MAX_BONE_INFLUENCE)};
    };

    class Uniforms
    {
    public:
        inline static const GPUVariableDefinitionData mSampler{GPUStorage::UNIFORM, PrimitiveTypes::mSampler2D, "uSampler"};
    };

    class SharedBuffers
    {
    public:

        class GPUGlobalData
        {
        public:
            Matrix4 mProjectionMatrix;
            Matrix4 mViewMatrix;
            alignas(16) Vector3 mCameraPosition;
            alignas(16) f32 mTime = 0;
            alignas(16) Vector2 mWindowSize;
        };

        inline static const GPUSharedBufferData mGlobalData
        {
            GPUBufferType::UNIFORM,
            {
                {GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "projectionMatrix"},
                {GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "viewMatrix"},
                {GPUStorage::UNIFORM, PrimitiveTypes::mVector3, "cameraPosition"},
                {GPUStorage::UNIFORM, PrimitiveTypes::mFloat, "time"},
                {GPUStorage::UNIFORM, PrimitiveTypes::mVector2, "windowSize"}
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

        inline static const GPUSharedBufferData mLightsData
        {
            GPUBufferType::UNIFORM,
            {
                {{GPUStorage::UNIFORM, StructTypes::mLight, "lights"}, "", std::to_string(10)},
                // {GPUStorage::UNIFORM, PrimitiveTypes::mFloat, "ambientIntensity"},
            },
            "LightsData",
            "lightsData"
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
        inline static const GPUVariableDefinitionData mMatrixID{GPUStorage::IN, PrimitiveTypes::mUnsignedInt, "in_matrixID"};
    };

    class VertexOutput
    {
    public:
        inline static const GPUVariableDefinitionData mPosition{GPUStorage::OUT, PrimitiveTypes::mVector4, "gl_Position"};
        inline static const GPUVariableDefinitionData mTextureCoord{GPUStorage::OUT, PrimitiveTypes::mVector2, "vTexcoord"};
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::OUT, PrimitiveTypes::mVector3, "vNormal"};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::OUT, PrimitiveTypes::mVector4, "vColor"};
        inline static const GPUVariableDefinitionData mFragPosition{GPUStorage::OUT, PrimitiveTypes::mVector3, "vPosition"};
        inline static const GPUVariableDefinitionData mInstanceID{GPUInterpolation::FLAT, GPUStorage::OUT, PrimitiveTypes::mUnsignedInt, "vInstanceID"};
    };

    class FragmentInput
    {
    public:
        inline static const GPUVariableDefinitionData mTextureCoord{GPUStorage::IN, VertexOutput::mTextureCoord};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::IN, VertexOutput::mColor};
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::IN, VertexOutput::mNormal};
        inline static const GPUVariableDefinitionData mFragPosition{GPUStorage::IN, VertexOutput::mFragPosition};
        inline static const GPUVariableDefinitionData mInstanceID{GPUInterpolation::FLAT, GPUStorage::IN, VertexOutput::mInstanceID};
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

        inline static const GPUFunctionDefinition mCalculatePhong
        {
            PrimitiveTypes::mVector4,
            "calculatePhong"
        };
    };
};