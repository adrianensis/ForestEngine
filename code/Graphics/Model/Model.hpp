#pragma once

#include "Engine/Core.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "cgltf.h"

class GPUSkeletalAnimation;
class GPUShader;
struct cgltf_data;
struct cgltf_accessor;
struct cgltf_primitive;
struct cgltf_node;
struct cgltf_material;
struct cgltf_skin;
struct cgltf_animation;
struct cgltf_animation_channel;
class Frame;


class GPUMesh;

class MeshInstanceData
{
public:
    Core::WeakPtr<const GPUMesh> mMesh;
    Maths::Matrix4 mMatrix;
};

class ModelManager;

class Model
{
public:
    void init(const std::string& path, ModelManager* modelManager);
    bool isAnimated() const { return mBonesIndexCount > 0 && !mSkeletalAnimations.empty(); }

    class KeyframeData
    {
    public:
        Core::f32 mPreviousTime = 0.0f;
        Core::f32 mNextTime = 0.0f;
        Core::u32 mKeyframe = 0;
        Core::f32 mInterpolationValue = 0;
    };

private:
    void loadGLTFShaders();
    void loadGLTFMeshes(ModelManager* modelManager);
    void loadGLTFPrimitive(const cgltf_primitive& primitive, ModelManager* modelManager);
    void loadGLTFBones(const cgltf_skin& skin);
    Core::f32 loadGLTFSkeletalAnimationDuration(const cgltf_animation& gltfAnim);
    void loadGLTFChannels(const cgltf_animation& gltfAnim);
    void loadGLTFSkeletalAnimationFrames(Core::WeakPtr<GPUSkeletalAnimation> animation);
    void loadGLTFSkeletalAnimations();
    static Maths::Matrix4 calculateHierarchicalBoneTransform(Core::u32 boneId, std::vector<Maths::Matrix4> originalFrameTransforms, const std::vector<GPUBoneData>& bones);
    static bool findKeyframeData(cgltf_accessor *input, Core::f32 currentTime, KeyframeData& keyframeData);
    static void getTranslationAtTime(cgltf_accessor *input, cgltf_interpolation_type interpolation, cgltf_accessor *output, Core::f32 currentTime, Maths::Vector3& out);
    static void getScaleAtTime(cgltf_accessor *input, cgltf_interpolation_type interpolation, cgltf_accessor *output, Core::f32 currentTime, Maths::Vector3& out);
    static void getRotationAtTime(cgltf_accessor *input, cgltf_interpolation_type interpolation, cgltf_accessor *output, Core::f32 currentTime, Maths::Quaternion& out);
    bool isSkinned() const;

private:
    class GLTFFace
    {
    public:
        GLTFFace(Core::u16 index0, Core::u16 index1, Core::u16 index2)
        {
            mIndices[0] = index0;
            mIndices[1] = index1;
            mIndices[2] = index2;
        }

    public:
        Core::u16 mIndices[3] {0,0,0};
    };

    template<class T>
    class GLTFBoneVertexIDsData
    {
    public:
        T mBonesIDs[GPUConstants::MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};
    };

    class GLTFChannels
    {
    public:
        cgltf_animation_channel *translate = nullptr;
        cgltf_animation_channel *rotate = nullptr;
        cgltf_animation_channel *scale = nullptr;
    };

private:
    cgltf_data* mCGLTFData = nullptr;
	std::filesystem::path mPath;
    std::vector<Core::OwnerPtr<GPUSkeletalAnimation>> mSkeletalAnimations;
    std::vector<MeshInstanceData> mMeshInstances;
    std::unordered_map<const cgltf_primitive*, Core::OwnerPtr<GPUMesh>> mGLTFMeshes;
    std::unordered_map<const cgltf_material*, GPUShader*> mGLTFShaders;
    std::unordered_map<Core::WeakPtr<const GPUMesh>, GPUShader*> mMeshShaders;
    std::unordered_map<const cgltf_node*, Core::u32> mNodeToBoneId;
    std::vector<GLTFChannels> mChannels;
    Core::u32 mBonesIndexCount = 0;
    GPUSkeletonState* mSkeletonState = nullptr;

public:
    CRGET(MeshInstances)
    CRGET(MeshShaders)
    GET(SkeletonState)
};
REGISTER_CLASS(Model);
