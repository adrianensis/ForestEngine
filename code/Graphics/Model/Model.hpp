#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/Mesh/GPUMesh.hpp"
#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "cgltf.h"

class GPUSkeletalAnimation;
class Material;
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
    Ptr<const GPUMesh> mMesh;
    Matrix4 mMatrix;
};

class Model: public ObjectBase
{
public:
    void init(const std::string& path);
    bool isAnimated() const { return mBonesIndexCount > 0 && !mSkeletalAnimations.empty(); }

    class KeyframeData
    {
    public:
        f32 mPreviousTime = 0.0f;
        f32 mNextTime = 0.0f;
        u32 mKeyframe = 0;
        f32 mInterpolationValue = 0;
    };

private:
    void loadGLTFMaterials();
    void loadGLTFMeshes();
    void loadGLTFPrimitive(const cgltf_primitive& primitive);
    void loadGLTFBones(const cgltf_skin& skin);
    f32 loadGLTFSkeletalAnimationDuration(const cgltf_animation& gltfAnim);
    void loadGLTFChannels(const cgltf_animation& gltfAnim);
    void loadGLTFSkeletalAnimationFrames(Ptr<GPUSkeletalAnimation> animation);
    void loadGLTFSkeletalAnimations();
    static Matrix4 calculateHierarchicalBoneTransform(u32 boneId, std::vector<Matrix4> originalFrameTransforms, const std::vector<GPUBoneData>& bones);
    static bool findKeyframeData(cgltf_accessor *input, f32 currentTime, KeyframeData& keyframeData);
    static void getTranslationAtTime(cgltf_accessor *input, cgltf_interpolation_type interpolation, cgltf_accessor *output, f32 currentTime, Vector3& out);
    static void getScaleAtTime(cgltf_accessor *input, cgltf_interpolation_type interpolation, cgltf_accessor *output, f32 currentTime, Vector3& out);
    static void getRotationAtTime(cgltf_accessor *input, cgltf_interpolation_type interpolation, cgltf_accessor *output, f32 currentTime, Quaternion& out);
    bool isSkinned() const;

private:
    class GLTFFace
    {
    public:
        GLTFFace(u16 index0, u16 index1, u16 index2)
        {
            mIndices[0] = index0;
            mIndices[1] = index1;
            mIndices[2] = index2;
        }

    public:
        u16 mIndices[3] {0,0,0};
    };

    template<class T>
    class GLTFBoneVertexIDsData
    {
    public:
        T mBonesIDs[GPUBuiltIn::MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};
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
    std::vector<OwnerPtr<GPUSkeletalAnimation>> mSkeletalAnimations;
    std::vector<MeshInstanceData> mMeshInstances;
    std::unordered_map<const cgltf_primitive*, OwnerPtr<GPUMesh>> mGLTFMeshes;
    std::unordered_map<const cgltf_material*, PoolHandler<Material>> mGLTFMaterials;
    std::unordered_map<Ptr<const GPUMesh>, PoolHandler<Material>> mMeshMaterials;
    std::unordered_map<const cgltf_node*, u32> mNodeToBoneId;
    std::vector<GLTFChannels> mChannels;
    u32 mBonesIndexCount = 0;
    Ptr<GPUSkeletonState> mSkeletonState;

public:
    CRGET(MeshInstances)
    CRGET(MeshMaterials)
    GET(SkeletonState)
};
REGISTER_CLASS(Model);
