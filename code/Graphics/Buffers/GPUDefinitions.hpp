#pragma once

#include "Graphics/Buffers/GPUBuffer.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class GPUBuiltIn
{
public:
    class Types
    {
    public:
        inline static const GPUDataType mBool{"bool",sizeof(bool), GPUBufferPrimitiveType::BOOL};
        inline static const GPUDataType mInt{"int",sizeof(i32), GPUBufferPrimitiveType::INT};
        inline static const GPUDataType mUnsignedInt{"uint",sizeof(u32), GPUBufferPrimitiveType::INT};
        inline static const GPUDataType mFloat{"float",sizeof(f32), GPUBufferPrimitiveType::FLOAT};
        inline static const GPUDataType mVector2{"vec2",sizeof(Vector2), GPUBufferPrimitiveType::FLOAT};
        inline static const GPUDataType mVector2i{"ivec2",sizeof(i32) * 4, GPUBufferPrimitiveType::INT};
        inline static const GPUDataType mVector3{"vec3",sizeof(Vector3), GPUBufferPrimitiveType::FLOAT};
        inline static const GPUDataType mVector3i{"ivec3",sizeof(i32) * 4, GPUBufferPrimitiveType::INT};
        inline static const GPUDataType mVector4{"vec4",sizeof(Vector4), GPUBufferPrimitiveType::FLOAT};
        inline static const GPUDataType mVector4i{"ivec4",sizeof(i32) * 4, GPUBufferPrimitiveType::INT};
        inline static const GPUDataType mMatrix4{"mat4",sizeof(Matrix4), GPUBufferPrimitiveType::FLOAT};
        inline static const GPUDataType mBoneIDs{"ivec4",sizeof(BoneVertexIDsData), GPUBufferPrimitiveType::INT};
        inline static const GPUDataType mBoneWeights{"vec4",sizeof(BoneVertexWeightsData), GPUBufferPrimitiveType::FLOAT};
        inline static const GPUDataType mSampler2D{"sampler2D",0, GPUBufferPrimitiveType::INT};
    };

    class Consts
    {
    public:
        // TODO: unsigned int?
        inline static const GPUVariableData MAX_BONES{Types::mInt, "MAX_BONES", "50"};
        inline static const GPUVariableData MAX_BONE_INFLUENCE{Types::mInt, "MAX_BONE_INFLUENCE", "4"};
    };

    class VertexInput
    {
    public:
        inline static const GPUVariableData mPosition{Types::mVector3, "position"};
        inline static const GPUVariableData mTextureCoord{Types::mVector2, "texcoord"};
        inline static const GPUVariableData mColor{Types::mVector4, "color"};
        inline static const GPUVariableData mModelMatrix{Types::mMatrix4, "modelMatrix"};
        inline static const GPUVariableData mBonesIDs{Types::mBoneIDs, "BoneIDs"};
        inline static const GPUVariableData mBonesWeights{Types::mBoneWeights, "Weights"};
    };

    class Uniforms
    {
    public:
        inline static const GPUVariableData mProjectionMatrix{Types::mMatrix4, "projectionMatrix"};
        inline static const GPUVariableData mViewMatrix{Types::mMatrix4, "viewMatrix"};
        inline static const GPUVariableData mIsInstanced{Types::mBool, "isInstanced"};
        inline static const GPUVariableData mHasTexture{Types::mBool, "hasTexture"};
        inline static const GPUVariableData mIsAnimated{Types::mBool, "isAnimated"};
        inline static const GPUVariableData mUseColorAsTint{Types::mBool, "useColorAsTint"};
        inline static const GPUVariableData mAlphaEnabled{Types::mBool, "alphaEnabled"};
        inline static const GPUVariableData mHasBorder{Types::mBool, "hasBorder"};
        inline static const GPUVariableData mTime{Types::mFloat, "time"};
        inline static const GPUVariableData mWindowSize{Types::mVector2, "windowSize"};
        inline static const GPUVariableData mBonesTransform{Types::mMatrix4, "bonesTransform", "", Consts::MAX_BONES.mName};
        inline static const GPUVariableData mBaseColor{Types::mVector4, "baseColor"};
        
        inline static const GPUVariableData mSampler{Types::mSampler2D, "uSampler"};
    };

    class VertexOutput
    {
    public:
        inline static const GPUVariableData mPosition{Types::mVector4, "gl_Position"};
        inline static const GPUVariableData mTextureCoord{Types::mVector2, "vTexcoord"};
        inline static const GPUVariableData mColor{Types::mVector4, "vColor"};
    };

    class FragmentOutput
    {
    public:
        inline static const GPUVariableData mColor{Types::mVector4, "FragColor"};
    };

    class Functions
    {
    public:
        inline static const GPUVariableData mTexture{Types::mVector2, "vTexcoord"};
        inline static const GPUVariableData mColor{Types::mVector4, "vColor"};
    };
};