#pragma once

#include "Graphics/Buffers/GPUAttribute.hpp"
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
        inline static const GPUAttributeData MAX_BONES{GPUStorage::CONST, {Types::mInt, "MAX_BONES", "50"}};
        inline static const GPUAttributeData MAX_BONE_INFLUENCE{GPUStorage::CONST, {Types::mInt, "MAX_BONE_INFLUENCE", "4"}};
    };

    class Uniforms
    {
    public:
        inline static const GPUAttributeData mProjectionMatrix{GPUStorage::UNIFORM, {Types::mMatrix4, "projectionMatrix"}};
        inline static const GPUAttributeData mViewMatrix{GPUStorage::UNIFORM, {Types::mMatrix4, "viewMatrix"}};
        inline static const GPUAttributeData mIsInstanced{GPUStorage::UNIFORM, {Types::mBool, "isInstanced"}};
        inline static const GPUAttributeData mHasTexture{GPUStorage::UNIFORM, {Types::mBool, "hasTexture"}};
        inline static const GPUAttributeData mIsAnimated{GPUStorage::UNIFORM, {Types::mBool, "isAnimated"}};
        inline static const GPUAttributeData mUseColorAsTint{GPUStorage::UNIFORM, {Types::mBool, "useColorAsTint"}};
        inline static const GPUAttributeData mAlphaEnabled{GPUStorage::UNIFORM, {Types::mBool, "alphaEnabled"}};
        inline static const GPUAttributeData mTime{GPUStorage::UNIFORM, {Types::mFloat, "time"}};
        inline static const GPUAttributeData mWindowSize{GPUStorage::UNIFORM, {Types::mVector2, "windowSize"}};
        inline static const GPUAttributeData mBonesTransform{GPUStorage::UNIFORM, {Types::mMatrix4, "bonesTransform", "", Consts::MAX_BONES.mName}};
        inline static const GPUAttributeData mBaseColor{GPUStorage::UNIFORM, {Types::mVector4, "baseColor"}};
        
        inline static const GPUAttributeData mSampler{GPUStorage::UNIFORM, {Types::mSampler2D, "uSampler"}};
    };

    class VertexInput
    {
    public:
        inline static const GPUAttributeData mPosition{GPUStorage::IN, {Types::mVector3, "position"}};
        inline static const GPUAttributeData mTextureCoord{GPUStorage::IN, {Types::mVector2, "texcoord"}};
        inline static const GPUAttributeData mColor{GPUStorage::IN, {Types::mVector4, "color"}};
        inline static const GPUAttributeData mModelMatrix{GPUStorage::IN, {Types::mMatrix4, "modelMatrix"}};
        inline static const GPUAttributeData mBonesIDs{GPUStorage::IN, {Types::mBoneIDs, "BoneIDs"}};
        inline static const GPUAttributeData mBonesWeights{GPUStorage::IN, {Types::mBoneWeights, "Weights"}};
    };

    class VertexOutput
    {
    public:
        inline static const GPUAttributeData mPosition{GPUStorage::OUT, {Types::mVector4, "gl_Position"}};
        inline static const GPUAttributeData mTextureCoord{GPUStorage::OUT, {Types::mVector2, "vTexcoord"}};
        inline static const GPUAttributeData mColor{GPUStorage::OUT, {Types::mVector4, "vColor"}};
    };

    class FragmentInput
    {
    public:
        inline static const GPUAttributeData mTextureCoord{GPUStorage::IN, VertexOutput::mTextureCoord};
        inline static const GPUAttributeData mColor{GPUStorage::IN, VertexOutput::mColor};
    };

    class FragmentOutput
    {
    public:
        inline static const GPUAttributeData mColor{GPUStorage::OUT, {Types::mVector4, "FragColor"}};
    };
};