#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Model/Animation/Animation.hpp"

class Animation;
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

class BoneData
{
public:

    i32 mId = -1;
    i32 mParentId = -1;
    std::string mName;
    // offset matrix transforms vertex from model space to bone space
	Matrix4 mBindMatrix;
};

class Mesh;

class Model: public ObjectBase
{
    GENERATE_METADATA(Model)

public:
    void init(const std::string& path);
    bool isAnimated() const { return mBonesIndexCount > 0 && !mAnimations.empty(); }

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
    f32 loadGLTFAnimationDuration(const cgltf_animation& gltfAnim);
    void loadGLTFChannels(const cgltf_animation& gltfAnim);
    void loadGLTFAnimationFrames(Ptr<Animation> animation);
    void loadGLTFAnimations();
    Matrix4 calculateHierarchicalBoneTransform(u32 boneId, std::vector<Matrix4> originalFrameTransforms) const;
    static bool findKeyframeData(cgltf_accessor *input, f32 currentTime, KeyframeData& keyframeData);
    static void getTranslationAtTime(cgltf_accessor *input, cgltf_accessor *output, f32 currentTime, Vector3& out);
    static void getScaleAtTime(cgltf_accessor *input, cgltf_accessor *output, f32 currentTime, Vector3& out);
    static void getRotationAtTime(cgltf_accessor *input, cgltf_accessor *output, f32 currentTime, Quaternion& out);

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

public:
    inline static const float smAnimationFPS = 60.0f;
    inline static const float smAnimationFrameRateSeconds = 1.0f/smAnimationFPS;

private:
    cgltf_data* mCGLTFData = nullptr;
	std::filesystem::path mPath;
    std::vector<OwnerPtr<Mesh>> mMeshes;
    std::vector<OwnerPtr<Animation>> mAnimations;
    std::unordered_map<cgltf_material*, Ptr<const Material>> mGLTFMaterials;
    std::unordered_map<Ptr<const Mesh>, Ptr<const Material>> mMeshMaterials;
    bool mIsSkinned = false;
    std::unordered_map<std::string, BoneData> mBonesMapping;
    std::vector<BoneData> mBones;
    std::vector<Matrix4> mOriginalFrameTransforms;
    std::vector<const cgltf_node*> mBonesToNode;
    std::unordered_map<const cgltf_node*, u32> mNodeToBoneId;
    std::vector<GLTFChannels> mChannels;
    u32 mBonesIndexCount = 0;

public:
    CRGET(Meshes)
    CRGET(MeshMaterials)
};
