#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"

class GPUBuiltIn
{
public:

    class PrimitiveTypes
    {
    public:
        inline static const GPUDataType mVoid{"void",0, GPUPrimitiveType::VOID};
        inline static const GPUDataType mBool{"bool",sizeof(bool), GPUPrimitiveType::BOOL};
        inline static const GPUDataType mInt{"int",sizeof(i32), GPUPrimitiveType::INT};
        inline static const GPUDataType mUnsignedInt{"uint",sizeof(u32), GPUPrimitiveType::INT};
        inline static const GPUDataType mFloat{"float",sizeof(f32), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mVector2{"vec2",sizeof(Vector2), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mVector2i{"ivec2",sizeof(i32) * 2, GPUPrimitiveType::INT};
        inline static const GPUDataType mVector3{"vec3",sizeof(Vector3), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mVector3i{"ivec3",sizeof(i32) * 3, GPUPrimitiveType::INT};
        inline static const GPUDataType mFace = mVector3i;
        inline static const GPUDataType mVector4{"vec4",sizeof(Vector4), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mVector4i{"ivec4",sizeof(i32) * 4, GPUPrimitiveType::INT};
        inline static const GPUDataType mMatrix4{"mat4",sizeof(Matrix4), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mSampler2D{"sampler2D",0, GPUPrimitiveType::INT};
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
        inline static const GPUDataType mLight{StructDefinitions::mLight.mName, StructDefinitions::mLight.getTypeSizeInBytes(), GPUPrimitiveType::STRUCT};
    };

    class Consts
    {
    public:
        // TODO: unsigned int?
        inline static const GPUVariableDefinitionData MAX_BONES{{GPUStorage::CONST, PrimitiveTypes::mInt, "MAX_BONES"}, "50"};
        inline static const GPUVariableDefinitionData MAX_BONE_INFLUENCE{{GPUStorage::CONST, PrimitiveTypes::mInt, "MAX_BONE_INFLUENCE"}, "4"};
    };

    class Uniforms
    {
    public:
        inline static const GPUVariableDefinitionData mTime{GPUStorage::UNIFORM, PrimitiveTypes::mFloat, "time"};
        inline static const GPUVariableDefinitionData mWindowSize{GPUStorage::UNIFORM, PrimitiveTypes::mVector2, "windowSize"};
        inline static const GPUVariableDefinitionData mBaseColor{GPUStorage::UNIFORM, PrimitiveTypes::mVector4, "baseColor"};
        
        inline static const GPUVariableDefinitionData mSampler{GPUStorage::UNIFORM, PrimitiveTypes::mSampler2D, "uSampler"};
    };

    class SharedBuffers
    {
    public:

        class GPUGlobalMatricesData
        {
        public:
            Matrix4 mProjectionMatrix;
            Matrix4 mViewMatrix;
        };

        inline static const GPUSharedBufferData mGlobalMatrices
        {
            GPUBufferType::UNIFORM,
            {
                {GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "projectionMatrix"},
                {GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "viewMatrix"}
            },
            "GlobalMatrices",
            "globalMatrices"
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
                {{GPUStorage::UNIFORM, PrimitiveTypes::mMatrix4, "bonesMatrices"}, "", "50"},
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
        inline static const GPUVariableDefinitionData mPosition{GPUStorage::IN, PrimitiveTypes::mVector3, "position"};
        inline static const GPUVariableDefinitionData mTextureCoord{GPUStorage::IN, PrimitiveTypes::mVector2, "texcoord"};
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::IN, PrimitiveTypes::mVector3, "normal"};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::IN, PrimitiveTypes::mVector4, "color"};
        inline static const GPUVariableDefinitionData mBonesIDs{GPUStorage::IN, PrimitiveTypes::mVector4i, "BoneIDs"};
        inline static const GPUVariableDefinitionData mBonesWeights{GPUStorage::IN, PrimitiveTypes::mVector4, "Weights"};
        inline static const GPUVariableDefinitionData mInstanceID{GPUStorage::IN, PrimitiveTypes::mUnsignedInt, "instanceID"};
    };

    class VertexOutput
    {
    public:
        inline static const GPUVariableDefinitionData mPosition{GPUStorage::OUT, PrimitiveTypes::mVector4, "gl_Position"};
        inline static const GPUVariableDefinitionData mTextureCoord{GPUStorage::OUT, PrimitiveTypes::mVector2, "vTexcoord"};
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::OUT, PrimitiveTypes::mVector3, "vNormal"};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::OUT, PrimitiveTypes::mVector4, "vColor"};
        inline static const GPUVariableDefinitionData mFragPosition{GPUStorage::OUT, PrimitiveTypes::mVector3, "vPosition"};
    };

    class FragmentInput
    {
    public:
        inline static const GPUVariableDefinitionData mTextureCoord{GPUStorage::IN, VertexOutput::mTextureCoord};
        inline static const GPUVariableDefinitionData mColor{GPUStorage::IN, VertexOutput::mColor};
        inline static const GPUVariableDefinitionData mNormal{GPUStorage::IN, VertexOutput::mNormal};
        inline static const GPUVariableDefinitionData mFragPosition{GPUStorage::IN, VertexOutput::mFragPosition};
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