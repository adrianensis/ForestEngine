#pragma once

#include "Graphics/GPU/GPUAttribute.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class GPUBuiltIn
{
public:
    class Types
    {
    public:
        inline static const GPUDataType mBool{"bool",sizeof(bool), GPUPrimitiveType::BOOL};
        inline static const GPUDataType mInt{"int",sizeof(i32), GPUPrimitiveType::INT};
        inline static const GPUDataType mUnsignedInt{"uint",sizeof(u32), GPUPrimitiveType::INT};
        inline static const GPUDataType mFloat{"float",sizeof(f32), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mVector2{"vec2",sizeof(Vector2), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mVector2i{"ivec2",sizeof(i32) * 2, GPUPrimitiveType::INT};
        inline static const GPUDataType mVector3{"vec3",sizeof(Vector3), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mVector3i{"ivec3",sizeof(i32) * 3, GPUPrimitiveType::INT};
        inline static const GPUDataType mVector4{"vec4",sizeof(Vector4), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mVector4i{"ivec4",sizeof(i32) * 4, GPUPrimitiveType::INT};
        inline static const GPUDataType mMatrix4{"mat4",sizeof(Matrix4), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mBoneIDs{"ivec4",sizeof(BoneVertexIDsData), GPUPrimitiveType::INT};
        inline static const GPUDataType mBoneWeights{"vec4",sizeof(BoneVertexWeightsData), GPUPrimitiveType::FLOAT};
        inline static const GPUDataType mSampler2D{"sampler2D",0, GPUPrimitiveType::INT};
    };

    class Consts
    {
    public:
        // TODO: unsigned int?
        inline static const GPUAttributeDefinitionData MAX_BONES{{GPUStorage::CONST, Types::mInt, "MAX_BONES"}, "50"};
        inline static const GPUAttributeDefinitionData MAX_BONE_INFLUENCE{{GPUStorage::CONST, Types::mInt, "MAX_BONE_INFLUENCE"}, "4"};
    };

    class Uniforms
    {
    public:
        inline static const GPUAttributeDefinitionData mProjectionMatrix{GPUStorage::UNIFORM, Types::mMatrix4, "projectionMatrix"};
        inline static const GPUAttributeDefinitionData mViewMatrix{GPUStorage::UNIFORM, Types::mMatrix4, "viewMatrix"};
        inline static const GPUAttributeDefinitionData mIsAnimated{GPUStorage::UNIFORM, Types::mBool, "isAnimated"};
        inline static const GPUAttributeDefinitionData mTime{GPUStorage::UNIFORM, Types::mFloat, "time"};
        inline static const GPUAttributeDefinitionData mWindowSize{GPUStorage::UNIFORM, Types::mVector2, "windowSize"};
        inline static const GPUAttributeDefinitionData mBonesTransform{{GPUStorage::UNIFORM, Types::mMatrix4, "bonesTransform"}, "", Consts::MAX_BONES.mName};
        inline static const GPUAttributeDefinitionData mBaseColor{GPUStorage::UNIFORM, Types::mVector4, "baseColor"};
        
        inline static const GPUAttributeDefinitionData mSampler{GPUStorage::UNIFORM, Types::mSampler2D, "uSampler"};
    };

    class VertexInput
    {
    public:
        inline static const GPUAttributeDefinitionData mPosition{GPUStorage::IN, Types::mVector3, "position"};
        inline static const GPUAttributeDefinitionData mTextureCoord{GPUStorage::IN, Types::mVector2, "texcoord"};
        inline static const GPUAttributeDefinitionData mColor{GPUStorage::IN, Types::mVector4, "color"};
        inline static const GPUAttributeDefinitionData mModelMatrix{GPUStorage::IN, Types::mMatrix4, "modelMatrix"};
        inline static const GPUAttributeDefinitionData mBonesIDs{GPUStorage::IN, Types::mBoneIDs, "BoneIDs"};
        inline static const GPUAttributeDefinitionData mBonesWeights{GPUStorage::IN, Types::mBoneWeights, "Weights"};
    };

    class VertexOutput
    {
    public:
        inline static const GPUAttributeDefinitionData mPosition{GPUStorage::OUT, Types::mVector4, "gl_Position"};
        inline static const GPUAttributeDefinitionData mTextureCoord{GPUStorage::OUT, Types::mVector2, "vTexcoord"};
        inline static const GPUAttributeDefinitionData mColor{GPUStorage::OUT, Types::mVector4, "vColor"};
    };

    class FragmentInput
    {
    public:
        inline static const GPUAttributeDefinitionData mTextureCoord{GPUStorage::IN, VertexOutput::mTextureCoord};
        inline static const GPUAttributeDefinitionData mColor{GPUStorage::IN, VertexOutput::mColor};
    };

    class FragmentOutput
    {
    public:
        inline static const GPUAttributeDefinitionData mColor{GPUStorage::OUT, Types::mVector4, "FragColor"};
    };
};