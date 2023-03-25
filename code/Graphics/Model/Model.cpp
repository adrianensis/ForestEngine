#include "Graphics/Model/Model.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Animation/Animation.hpp"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

Model::~Model()
{
    //cgltf_free(mCGLTFData);
}

void Model::init(const std::string& path)
{
    mPath = Paths::mResources + path;

    // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#skins
    // https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_007_Animations.md

    cgltf_options options;
	std::memset(&options, 0, sizeof(cgltf_options));
	cgltf_result result = cgltf_parse_file(&options, mPath.c_str(), &mCGLTFData);

	if (result == cgltf_result_success)
    {
		result = cgltf_load_buffers(&options, mCGLTFData, mPath.c_str());
    }

	if (result == cgltf_result_success)
    {
		result = cgltf_validate(mCGLTFData);
    }

    if (result == cgltf_result_success)
	{
        std::unordered_map<cgltf_material*, Ptr<Material>> cgltfMaterials;

        if(mCGLTFData->materials_count > 0)
        {
            FOR_RANGE(materialIt, 0, mCGLTFData->materials_count)
            {
                cgltf_material& cgltfMaterial = mCGLTFData->materials[materialIt];
                Ptr<Material> newMaterial;
                if(! MAP_CONTAINS(cgltfMaterials, &cgltfMaterial))
                {
                    newMaterial = GET_SYSTEM(MaterialManager).createMaterial();
                    MAP_INSERT(cgltfMaterials, &cgltfMaterial, newMaterial )
                }

                if(cgltfMaterial.has_pbr_metallic_roughness)
                {
                    std::filesystem::path texturePath = mPath.parent_path().append(cgltfMaterial.pbr_metallic_roughness.base_color_texture.texture->image->uri);
                    newMaterial.get().mTextures[(u32)TextureType::BASE_COLOR] = GET_SYSTEM(MaterialManager).loadTexture(texturePath);
                }
            }
        }

        if(mCGLTFData->meshes_count > 0)
        {
            FOR_RANGE(meshIt, 0, mCGLTFData->meshes_count)
            {
                cgltf_mesh& cgltfMesh = mCGLTFData->meshes[meshIt];

                FOR_RANGE(primitiveIt, 0, cgltfMesh.primitives_count)
                {
                    cgltf_primitive& primitive = cgltfMesh.primitives[primitiveIt];
                    ASSERT_MSG(primitive.type == cgltf_primitive_type::cgltf_primitive_type_triangles, "Mesh has to be made out of triangles!")

                    OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>::newObject();
                    mesh.get().mModel = (getPtrToThis());
                    mMeshes.push_back(mesh);

                    FOR_RANGE(attributeIt, 0, primitive.attributes_count)
                    {
                        cgltf_attribute& attribute = primitive.attributes[attributeIt];
                        if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_position)
                        {
                            mesh.get().init(attribute.data->count, primitive.indices->count / 3);
                            break;
                        }
                    }

                    mesh.get().setColor(Vector4(0,0,0,1));

                    mesh.get().mMaterial = cgltfMaterials[primitive.material];

                    FOR_RANGE(attributeIt, 0, primitive.attributes_count)
                    {
                        cgltf_attribute& attribute = primitive.attributes[attributeIt];
                        if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_position)
                        {
                            FOR_RANGE(vertexIt, 0, attribute.data->count)
                            {
                                Vector3* positionsArray = reinterpret_cast<Vector3*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                                Vector3& position = positionsArray[vertexIt];
                                mesh.get().addToPositions(position);
                            }
                        }
                        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_texcoord)
                        {
                            FOR_RANGE(vertexIt, 0, attribute.data->count)
                            {
                                Vector2* texCoordArray = reinterpret_cast<Vector2*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                                Vector2& texCoord = texCoordArray[vertexIt];
                                mesh.get().addToTextureCoordinates(texCoord);
                            }
                        }
                        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_joints)
                        {
                            if ((attribute.data->component_type == cgltf_component_type_r_8u) && (attribute.data->type == cgltf_type_vec4))
                            {
                                FOR_RANGE(vertexIt, 0, attribute.data->count)
                                {
                                    GLTFBoneVertexIDsData* boneVertexIDsDataU8Array = reinterpret_cast<GLTFBoneVertexIDsData*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                                    GLTFBoneVertexIDsData& boneVertexIDsDataU8 = boneVertexIDsDataU8Array[vertexIt];

                                    BoneVertexIDsData boneVertexIDsData;
                                    FOR_RANGE(i, 0, smMaxBonesPerVertex)
                                    {
                                        boneVertexIDsData.mBonesIDs[i] = boneVertexIDsDataU8.mBonesIDs[i];
                                    }

                                    mesh.get().addToBonesVertexIDsData(boneVertexIDsData);
                                }
                            }
                            else
                            {
                                ASSERT_MSG(false, "Joint attribute data format not supported, use vec4 u8")
                            }
                        }
                        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_weights)
                        {
                            if ((attribute.data->component_type == cgltf_component_type_r_32f) && (attribute.data->type == cgltf_type_vec4))
                            {
                                FOR_RANGE(vertexIt, 0, attribute.data->count)
                                {
                                    BoneVertexWeightsData* boneVertexWeightsDataArray = reinterpret_cast<BoneVertexWeightsData*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                                    BoneVertexWeightsData& boneVertexWeightsData = boneVertexWeightsDataArray[vertexIt];
                                    mesh.get().addToBonesVertexWeightsData(boneVertexWeightsData);
                                }
                            }
                        }
                    }

                    FOR_RANGE(faceIt, 0, primitive.indices->count / 3)
                    {
                        GLTFFace* facesArray = reinterpret_cast<GLTFFace*>(reinterpret_cast<byte*>(primitive.indices->buffer_view->buffer->data) + primitive.indices->offset + primitive.indices->buffer_view->offset);
                        GLTFFace& face = facesArray[faceIt];
                        mesh.get().addToFaces(Face(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
                    }
                }
            }

            if (mCGLTFData->skins_count == 1)
            {
                cgltf_skin& skin = mCGLTFData->skins[0];

                mBonesIndexCount = (u32)skin.joints_count;
                mBones.reserve(mBonesIndexCount);
                mChannels.resize(mBonesIndexCount);
                mBonesToNode.resize(mBonesIndexCount);

                for (unsigned int i = 0; i < mBonesIndexCount; i++)
                {
                    cgltf_node& node = *skin.joints[i];
                    mBonesToNode[i] = &node;
                    std::string boneName(node.name);

                    if (! MAP_CONTAINS(mBonesMapping, boneName)) 
                    {
                        BoneData boneData;
                        boneData.mId = i;

                        Matrix4 translationMatrix;
                        translationMatrix.identity();
                        if(node.has_translation)
                        {
                            translationMatrix.translation(Vector3(node.translation[0], node.translation[1], node.translation[2]));
                        }

                        Matrix4 rotationMatrix;
                        rotationMatrix.identity();
                        if(node.has_rotation)
                        {
                            Quaternion rotation(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
                            //rotationMatrix.rotation(Vector3(node.rotation[0], node.rotation[1], node.rotation[2]));
                            rotation.toMatrix(&rotationMatrix);
                            rotationMatrix.transpose();
                        }

                        Matrix4 scaleMatrix;
                        scaleMatrix.identity();
                        if(node.has_scale)
                        {
                            scaleMatrix.scale(Vector3(node.scale[0], node.scale[1], node.scale[2]));
                        }
    
                        Matrix4 boneMatrix = translationMatrix;
                        rotationMatrix.mul(scaleMatrix);
                        boneMatrix.mul(rotationMatrix);

                        boneData.mBindMatrix = boneMatrix;

                        // Find parent bone index
                        unsigned int parentIndex = -1;

                        for (unsigned int j = 0; j < mBonesIndexCount; j++)
                        {
                            if (skin.joints[j] == node.parent)
                            {
                                parentIndex = j;
                                break;
                            }
                        }

                        boneData.mParentId = parentIndex;
                        boneData.mName = boneName;

                        MAP_INSERT(mBonesMapping, boneName, boneData);
                        mBones.push_back(boneData);
                    }
                }

                buildBindPoseHierarchy();

                FOR_RANGE(animIt, 0, mCGLTFData->animations_count)
                {
                    cgltf_animation& gltfAnim = mCGLTFData->animations[animIt];
                    f32 animDuration = 0.0f;
                    FOR_RANGE(channelIt, 0, gltfAnim.channels_count)
                    {
                        cgltf_animation_channel& channel = gltfAnim.channels[channelIt];
                        ASSERT_MSG(channel.sampler->interpolation == cgltf_interpolation_type_linear, "Interpolation is not linear.");
                        
                        i32 boneIndex = -1;

                        for (unsigned int k = 0; k < mBonesIndexCount; k++)
                        {
                            if (channel.target_node == skin.joints[k])
                            {
                                boneIndex = k;
                                break;
                            }
                        }

                        if (boneIndex == -1)
                        {
                            // Animation channel for a node not in the armature
                            continue;
                        }

                        if (channel.target_path == cgltf_animation_path_type_translation)
                        {
                            mChannels[boneIndex].translate = &channel;
                        }
                        else if (channel.target_path == cgltf_animation_path_type_rotation)
                        {
                            mChannels[boneIndex].rotate = &channel;
                        }
                        else if (channel.target_path == cgltf_animation_path_type_scale)
                        {
                            mChannels[boneIndex].scale = &channel;
                        }

                        float t = channel.sampler->input->max[0];
                        animDuration = (t > animDuration)? t : animDuration;
                    }

                    OwnerPtr<Animation> animation = OwnerPtr<Animation>::newObject();
                    animation.get().init(animIt, getPtrToThis());

                    // TODO: put this inside Animation init!
                    // ------
                    animation.get().mDurationInSeconds = animDuration;
                    animation.get().mTicksPerSecond = smAnimationFPS;
                    animation.get().mDurationInTicks = (int)(animDuration/smAnimationFrameRateSeconds);
                    animation.get().mFrames.resize(animation.get().mDurationInTicks);
                    // ------

                    FOR_RANGE(frameIt, 0, animation.get().mDurationInTicks)
                    {
                        animation.get().mFrames[frameIt].mTransforms.resize(mBonesIndexCount);
                    }

                    // https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_007_Animations.md

                    FOR_RANGE(frameIt, 0, animation.get().mDurationInTicks)
                    {
                        f32 currentAnimationTime = frameIt*smAnimationFrameRateSeconds;
                        FOR_RANGE(boneIt, 0, mBonesIndexCount)
                        {
                            Vector3 translation(0, 0, 0);
                            if (mChannels[boneIt].translate)
                            {
                                getTranslationAtTime(mChannels[boneIt].translate->sampler->input, mChannels[boneIt].translate->sampler->output, currentAnimationTime, translation);
                            }
                            
                            Quaternion rotation(0, 0, 0, 1);
                            if (mChannels[boneIt].rotate)
                            {
                                getRotationAtTime(mChannels[boneIt].rotate->sampler->input, mChannels[boneIt].rotate->sampler->output, currentAnimationTime, rotation);
                            }

                            Vector3 scale(1, 1, 1);
                            if (mChannels[boneIt].scale)
                            {
                                getScaleAtTime(mChannels[boneIt].scale->sampler->input, mChannels[boneIt].scale->sampler->output, currentAnimationTime, scale);
                            }

                            Matrix4 translationMatrix;
                            translationMatrix.translation(translation);
                            Matrix4 rotationMatrix; 
                            rotation.toMatrix(&rotationMatrix);
                            rotationMatrix.transpose();
                            // rotationMatrix.rotation(rotation.toEuler());
                            Matrix4 scaleMatrix;
                            scaleMatrix.scale(scale);

                            Matrix4 boneFrameMatrix = translationMatrix;
                            rotationMatrix.mul(scaleMatrix);
                            boneFrameMatrix.mul(rotationMatrix);

                            animation.get().mFrames[frameIt].mTransforms[boneIt] = boneFrameMatrix;
                        }

                        buildFrameHierarchy(animation.get().mFrames[frameIt]);

                        FOR_RANGE(boneIt, 0, mBonesIndexCount)
                        {
                            Matrix4 boneFrameMatrix = animation.get().mFrames[frameIt].mTransforms[boneIt];
                            Matrix4 inverseBindMatrix = mBones[boneIt].mBindMatrix;
                            inverseBindMatrix.invert();
                            boneFrameMatrix.mul(inverseBindMatrix);

                            animation.get().mFrames[frameIt].mTransforms[boneIt] = boneFrameMatrix;
                        }
                    }

                    mAnimations.emplace_back(animation);

                    GET_SYSTEM(AnimationManager).createAnimationState(animation);
                }

                mChannels.clear();
                mBonesToNode.clear();
            }
        }
    }

    cgltf_free(mCGLTFData);
}

void Model::buildFrameHierarchy(Frame& frame)
{
    for (int i = 0; i < mBonesIndexCount; i++)
    {
        if (mBones[i].mParentId >= 0)
        {
            if (mBones[i].mParentId > i)
            {
                ASSERT_MSG(false, "Bones are not sorted!");
                //continue;
            }

            Matrix4 finalTransform = frame.mTransforms[mBones[i].mParentId];
            finalTransform.mul(frame.mTransforms[i]);
            frame.mTransforms[i] = finalTransform;
        }
    }
}

void Model::buildBindPoseHierarchy()
{
    for (int i = 0; i < mBonesIndexCount; i++)
    {
        if (mBones[i].mParentId >= 0)
        {
            if (mBones[i].mParentId > i)
            {
                ASSERT_MSG(false, "Bones are not sorted!");
                //continue;
            }

            Matrix4 finalTransform = mBones[mBones[i].mParentId].mBindMatrix;
            finalTransform.mul(mBones[i].mBindMatrix);
            mBones[i].mBindMatrix = finalTransform;
        }
    }
}

bool Model::findKeyframeData(cgltf_accessor *input, f32 currentTime, KeyframeData& keyframeData)
{
    keyframeData.mPreviousTime = 0.0f;
    keyframeData.mNextTime = 0.0f;
    keyframeData.mKeyframe = 0;
    bool keyframeFound = false;

    FOR_RANGE_COND(i, 0, input->count - 1, !keyframeFound)
    {
        if (!cgltf_accessor_read_float(input, i, &keyframeData.mPreviousTime, 1))
        {
            return false;
        }

        if (!cgltf_accessor_read_float(input, i + 1, &keyframeData.mNextTime, 1))
        {
            return false;
        }

        if ((keyframeData.mPreviousTime <= currentTime) && (currentTime < keyframeData.mNextTime))
        {
            keyframeData.mKeyframe = i;
            keyframeFound = true;
        }
    }

    keyframeData.mInterpolationValue = (currentTime - keyframeData.mPreviousTime) / (keyframeData.mNextTime - keyframeData.mPreviousTime);
    keyframeData.mInterpolationValue = std::clamp(keyframeData.mInterpolationValue, 0.0f, 1.0f);

    return true;
}

void Model::getTranslationAtTime(cgltf_accessor *input, cgltf_accessor *output, f32 currentTime, Vector3& out)
{
    KeyframeData keyframeData;
    bool result = findKeyframeData(input, currentTime, keyframeData);

    ASSERT_MSG(result, "Error obtaining keyframe data!");
    ASSERT_MSG(output->component_type == cgltf_component_type_r_32f, "Component type is not f32!");
    ASSERT_MSG(output->type == cgltf_type_vec3, "Type is not vec3!");

    Vector3 v1;
    Vector3 v2;
    cgltf_accessor_read_float(output, keyframeData.mKeyframe, (cgltf_float*)&v1, 3);
    cgltf_accessor_read_float(output, keyframeData.mKeyframe+1, (cgltf_float*)&v2, 3);
    out = v1;
    out.lerp(v2, keyframeData.mInterpolationValue);
}
void Model::getScaleAtTime(cgltf_accessor *input, cgltf_accessor *output, f32 currentTime, Vector3& out)
{
    // use same code as Translation
    return getTranslationAtTime(input, output, currentTime, out);
}
void Model::getRotationAtTime(cgltf_accessor *input, cgltf_accessor *output, f32 currentTime, Quaternion& out)
{
    KeyframeData keyframeData;
    bool result = findKeyframeData(input, currentTime, keyframeData);

    ASSERT_MSG(result, "Error obtaining keyframe data!");
    ASSERT_MSG(output->component_type == cgltf_component_type_r_32f, "Component type is not f32!");
    ASSERT_MSG(output->type == cgltf_type_vec4, "Type is not vec4!");
    
    Quaternion q1;
    Quaternion q2;
    cgltf_accessor_read_float(output, keyframeData.mKeyframe, (cgltf_float*)&q1, 4);
    cgltf_accessor_read_float(output, keyframeData.mKeyframe+1, (cgltf_float*)&q2, 4);
    out = q1;
    out.slerp(q2, keyframeData.mInterpolationValue);
}