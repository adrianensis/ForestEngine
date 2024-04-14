#include "Graphics/Model/Model.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/MaterialRuntimeDefault.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Animation/Animation.hpp"
#include "Engine/Assets/Config/Paths.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

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
        CHECK_MSG(mCGLTFData->skins_count <= 1, "Only 1 Skin allowed!")

        loadGLTFMaterials();

        if(mCGLTFData->meshes_count > 0)
        {
            loadGLTFMeshes();

            if (isSkinned())
            {
                const cgltf_skin& skin = mCGLTFData->skins[0];
                loadGLTFBones(skin);
                loadGLTFAnimations();

                mGLTFMaterials.clear();
                mChannels.clear();
                mInverseBindMatrices.clear();
                mNodeToBoneId.clear();
            }
        }
    }

    cgltf_free(mCGLTFData);
}

bool Model::isSkinned() const
{
    return mCGLTFData->skins_count == 1;
}

void Model::loadGLTFMaterials()
{
    if(mCGLTFData->materials_count > 0)
    {
        FOR_RANGE(materialIt, 0, mCGLTFData->materials_count)
        {
            cgltf_material& cgltfMaterial = mCGLTFData->materials[materialIt];
            MaterialData materialData;
            materialData.mIsSkinned = isSkinned();
            materialData.mAlphaEnabled = false;
            materialData.mCullFaceType = cgltfMaterial.double_sided ? GPUCullFaceType::BACK : GPUCullFaceType::NONE;
            // materialData.mUseColorAsTint = true;
            PoolHandler<Material> newMaterial;

            if(cgltfMaterial.has_pbr_metallic_roughness)
            {
                materialData.mSharedMaterialPropertiesBlockBuffer.set<MaterialPropertiesBlockPBRMetallicRoughness>();

                if(cgltfMaterial.pbr_metallic_roughness.base_color_texture.texture)
                {
                    std::filesystem::path texturePath = mPath.parent_path().append(cgltfMaterial.pbr_metallic_roughness.base_color_texture.texture->image->uri);
                    materialData.mTextureBindings[(u32)TextureMap::BASE_COLOR] = MaterialTextureBinding{texturePath, GPUPipelineStage::FRAGMENT};
                }
                else
                {
                    cgltf_float* baseColor = cgltfMaterial.pbr_metallic_roughness.base_color_factor;
                    materialData.mSharedMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockPBRMetallicRoughness>().mBaseColor = Vector4(baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
                }
                if(cgltfMaterial.pbr_metallic_roughness.metallic_roughness_texture.texture)
                {
                    std::filesystem::path texturePath = mPath.parent_path().append(cgltfMaterial.pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri);
                    materialData.mTextureBindings[(u32)TextureMap::METALLIC_ROUGHNESS] = MaterialTextureBinding{texturePath, GPUPipelineStage::FRAGMENT};
                }
                else
                {
                    cgltf_float metallic = cgltfMaterial.pbr_metallic_roughness.metallic_factor;
                    cgltf_float roughness = cgltfMaterial.pbr_metallic_roughness.roughness_factor;
                    // materialData.mSharedMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockPBRMetallicRoughness>().mMetallic = Vector3(metallic, metallic, metallic);
                    // materialData.mSharedMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockPBRMetallicRoughness>().mRoughness = Vector3(roughness, roughness, roughness);
                }

                newMaterial = GET_SYSTEM(MaterialManager).createMaterial<MaterialRuntimePBRMetallicRoughness>(materialData);
            }

            // if(cgltfMaterial.has_pbr_specular_glossiness)
            // {
            //     materialData.mSharedMaterialPropertiesBlockBuffer.set<MaterialPropertiesBlockPBRSpecularGlossiness>();
            //     materialRuntime = new MaterialRuntimePBRSpecularGlossiness();

            //     if(cgltfMaterial.pbr_specular_glossiness.diffuse_texture.texture)
            //     {
            //         std::filesystem::path texturePath = mPath.parent_path().append(cgltfMaterial.pbr_specular_glossiness.diffuse_texture.texture->image->uri);
            //         materialData.mTextureBindings[(u32)TextureMap::DIFFUSE] = MaterialTextureBinding{texturePath, GPUPipelineStage::FRAGMENT};
            //     }
            //     else
            //     {
            //         cgltf_float* diffuse = cgltfMaterial.pbr_specular_glossiness.diffuse_factor;
            //         materialData.mSharedMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockPBRSpecularGlossiness>().mDiffuse = Vector3(diffuse[0], diffuse[1], diffuse[2]);
            //     }
            //     if(cgltfMaterial.pbr_specular_glossiness.specular_glossiness_texture.texture)
            //     {
            //         std::filesystem::path texturePath = mPath.parent_path().append(cgltfMaterial.pbr_specular_glossiness.specular_glossiness_texture.texture->image->uri);
            //         materialData.mTextureBindings[(u32)TextureMap::SPECULAR_GLOSSINESS] = MaterialTextureBinding{texturePath, GPUPipelineStage::FRAGMENT};
            //     }
            //     else
            //     {
            //         cgltf_float* specular = cgltfMaterial.pbr_specular_glossiness.specular_factor;
            //         cgltf_float glossiness = cgltfMaterial.pbr_specular_glossiness.glossiness_factor;
            //         materialData.mSharedMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockPBRSpecularGlossiness>().mDiffuse = Vector3(specular[0], specular[1], specular[2]);
            //         materialData.mSharedMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockPBRSpecularGlossiness>().mGlossiness = Vector3(glossiness, glossiness, glossiness);
            //     }
            // }

            if(! mGLTFMaterials.contains(&cgltfMaterial))
            {
                mGLTFMaterials.insert_or_assign(&cgltfMaterial, newMaterial);
            }
        }
    }
}

void Model::loadGLTFMeshes()
{
    FOR_RANGE(meshIt, 0, mCGLTFData->meshes_count)
    {
        cgltf_mesh& cgltfMesh = mCGLTFData->meshes[meshIt];
        FOR_RANGE(primitiveIt, 0, cgltfMesh.primitives_count)
        {
            loadGLTFPrimitive(cgltfMesh.primitives[primitiveIt]);
        }
    }
}

void Model::loadGLTFPrimitive(const cgltf_primitive& primitive)
{
    CHECK_MSG(primitive.type == cgltf_primitive_type::cgltf_primitive_type_triangles, "Mesh has to be made out of triangles!")

    Ptr<Mesh> mesh = mMeshes.emplace_back(OwnerPtr<Mesh>::newObject());
    mesh->mModel = (getPtrToThis<Model>());

    PoolHandler<Material> meshMaterial;
    if(primitive.material)
    {
        meshMaterial = mGLTFMaterials[primitive.material];
    }

    mMeshMaterials[mesh] = meshMaterial;

    std::vector<GPUVariableData> gpuVertexInputBuffers;
    FOR_RANGE(attributeIt, 0, primitive.attributes_count)
    {
        cgltf_attribute& attribute = primitive.attributes[attributeIt];
        if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_position)
        {
            gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mPosition);
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_texcoord)
        {
            gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mTextureCoord);
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_color)
        {
            gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mColor);
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_normal)
        {
            gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mNormal);
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_joints)
        {
            gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mBonesIDs);
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_weights)
        {
            gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mBonesWeights);
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_tangent)
        {
            // TODO: implement
        }
        else
        {
            CHECK_MSG(false, "Attribute not supported!");
        }
    }

    FOR_RANGE(attributeIt, 0, primitive.attributes_count)
    {
        cgltf_attribute& attribute = primitive.attributes[attributeIt];
        if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_position)
        {
            mesh->init(attribute.data->count, primitive.indices->count / 3, gpuVertexInputBuffers);
            break;
        }
    }

    FOR_RANGE(attributeIt, 0, primitive.attributes_count)
    {
        cgltf_attribute& attribute = primitive.attributes[attributeIt];
        if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_position)
        {
            mesh->mMin = Vector3(attribute.data->min[0], attribute.data->min[1], attribute.data->min[2]);
            mesh->mMax = Vector3(attribute.data->max[0], attribute.data->max[1], attribute.data->max[2]);

            FOR_RANGE(vertexIt, 0, attribute.data->count)
            {
                Vector3* positionsArray = reinterpret_cast<Vector3*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                Vector3& position = positionsArray[vertexIt];
                mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(position);
            }
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_texcoord)
        {
            FOR_RANGE(vertexIt, 0, attribute.data->count)
            {
                Vector2* texCoordArray = reinterpret_cast<Vector2*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                Vector2& texCoord = texCoordArray[vertexIt];
                texCoord.y = 1.0f - texCoord.y;
                mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoord.mName).pushBack(texCoord);
            }
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_color)
        {
            FOR_RANGE(vertexIt, 0, attribute.data->count)
            {
                Vector4* colorArray = reinterpret_cast<Vector4*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                Vector4& color = colorArray[vertexIt];
                mesh->mBuffers.at(GPUBuiltIn::VertexInput::mColor.mName).pushBack(color);
            }
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_normal)
        {
            FOR_RANGE(vertexIt, 0, attribute.data->count)
            {
                Vector3* normalArray = reinterpret_cast<Vector3*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                Vector3& normal = normalArray[vertexIt];
                mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(normal);
            }
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_joints)
        {
            if ((attribute.data->component_type == cgltf_component_type_r_8u) && (attribute.data->type == cgltf_type_vec4))
            {
                FOR_RANGE(vertexIt, 0, attribute.data->count)
                {
                    GLTFBoneVertexIDsData<i8>* boneVertexIDsDataU8Array = reinterpret_cast<GLTFBoneVertexIDsData<i8>*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                    GLTFBoneVertexIDsData<i8>& boneVertexIDsDataU8 = boneVertexIDsDataU8Array[vertexIt];

                    BoneVertexIDsData boneVertexIDsData;
                    FOR_RANGE(i, 0, GPUBuiltIn::MAX_BONE_INFLUENCE)
                    {
                        boneVertexIDsData.mBonesIDs[i] = boneVertexIDsDataU8.mBonesIDs[i];
                    }

                    mesh->mBuffers.at(GPUBuiltIn::VertexInput::mBonesIDs.mName).pushBack(boneVertexIDsData);
                }
            }
            else if ((attribute.data->component_type == cgltf_component_type_r_16u) && (attribute.data->type == cgltf_type_vec4))
            {
                FOR_RANGE(vertexIt, 0, attribute.data->count)
                {
                    GLTFBoneVertexIDsData<i16>* boneVertexIDsDataU16Array = reinterpret_cast<GLTFBoneVertexIDsData<i16>*>(reinterpret_cast<byte*>(attribute.data->buffer_view->buffer->data) + attribute.data->offset + attribute.data->buffer_view->offset);
                    GLTFBoneVertexIDsData<i16>& boneVertexIDsDataU16 = boneVertexIDsDataU16Array[vertexIt];

                    BoneVertexIDsData boneVertexIDsData;
                    FOR_RANGE(i, 0, GPUBuiltIn::MAX_BONE_INFLUENCE)
                    {
                        boneVertexIDsData.mBonesIDs[i] = boneVertexIDsDataU16.mBonesIDs[i];
                    }

                    mesh->mBuffers.at(GPUBuiltIn::VertexInput::mBonesIDs.mName).pushBack(boneVertexIDsData);
                }
            }
            else
            {
                CHECK_MSG(false, "Joint attribute data format not supported, use vec4 u8 or u16");
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
                    mesh->mBuffers.at(GPUBuiltIn::VertexInput::mBonesWeights.mName).pushBack(boneVertexWeightsData);
                }
            }
            else
            {
                CHECK_MSG(false, "Weights attribute data format not supported, use vec4 f32");
            }
        }
        else if(attribute.type == cgltf_attribute_type::cgltf_attribute_type_tangent)
        {
            // TODO: implement
        }
        else
        {
            CHECK_MSG(false, "Attribute not supported!");
        }
    }

    FOR_RANGE(faceIt, 0, primitive.indices->count / 3)
    {
        GLTFFace* indicesArray = reinterpret_cast<GLTFFace*>(reinterpret_cast<byte*>(primitive.indices->buffer_view->buffer->data) + primitive.indices->offset + primitive.indices->buffer_view->offset);
        GLTFFace& face = indicesArray[faceIt];
        mesh->mIndices.pushBack(Face(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
    }
}

void Model::loadGLTFBones(const cgltf_skin& skin)
{
    mBonesIndexCount = (u32)skin.joints_count;
    mBones.reserve(mBonesIndexCount);
    mChannels.resize(mBonesIndexCount);
    mInverseBindMatrices.resize(mBonesIndexCount);

    std::vector<Matrix4> originalBindMatrices;
    originalBindMatrices.resize(mBonesIndexCount);

    FOR_RANGE(i, 0, mBonesIndexCount)
    {
        if(skin.inverse_bind_matrices)
        {
            f32 inverseMatrixData[Matrix4::smMatrixSize];
            cgltf_accessor_read_float(skin.inverse_bind_matrices, i, inverseMatrixData, Matrix4::smMatrixSize);
            Matrix4 inverse;
            inverse.init(inverseMatrixData);
            mInverseBindMatrices[i]  = inverse;
        }
        else
        {
            CHECK_MSG(false, "No inverse bind matrices!");
        }

        const cgltf_node& node = *skin.joints[i];
        if (! mNodeToBoneId.contains(&node)) 
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
                rotation.toMatrix(&rotationMatrix);
            }

            Matrix4 scaleMatrix;
            scaleMatrix.identity();
            if(node.has_scale)
            {
                scaleMatrix.scale(Vector3(node.scale[0], node.scale[1], node.scale[2]));
            }

            Matrix4 bindMatrix = translationMatrix;
            rotationMatrix.mul(scaleMatrix);
            bindMatrix.mul(rotationMatrix);

            boneData.mBindMatrix = bindMatrix;
            originalBindMatrices[i] = bindMatrix;

            // Find parent bone index
            i32 parentIndex = INVALID_INDEX;

            FOR_RANGE(j, 0, mBonesIndexCount)
            {
                if (skin.joints[j] == node.parent)
                {
                    parentIndex = j;
                    CHECK_MSG(parentIndex != i, "Bone cannot be its own parent!");
                    CHECK_MSG(parentIndex < i, "Bones are not sorted!");
                    break;
                }
            }

            mNodeToBoneId.insert_or_assign(&node, i);
            std::string boneName;

            if(node.name)
            {
                boneName = std::string((node.name));
            }

            boneData.mParentId = parentIndex;
            boneData.mName = boneName;

            mBones.push_back(boneData);
        }
    }

    FOR_RANGE(i, 0, mBonesIndexCount)
    {
        mBones[i].mBindMatrix = calculateHierarchicalBoneTransform(i, originalBindMatrices);
    }
}

f32 Model::loadGLTFAnimationDuration(const cgltf_animation& gltfAnim)
{
    f32 animDuration = 0.0f;
    FOR_RANGE(channelIt, 0, gltfAnim.channels_count)
    {
        cgltf_animation_channel& channel = gltfAnim.channels[channelIt];
        f32 samplerDuration = channel.sampler->input->max[0];
        animDuration = std::max(samplerDuration, animDuration);
    }
    return animDuration;
}

void Model::loadGLTFChannels(const cgltf_animation& gltfAnim)
{
    FOR_RANGE(channelIt, 0, gltfAnim.channels_count)
    {
        cgltf_animation_channel& channel = gltfAnim.channels[channelIt];

        i32 boneIndex = INVALID_INDEX;
        if(mNodeToBoneId.contains(channel.target_node))
        {
            boneIndex = mNodeToBoneId.at(channel.target_node);
        }
        else
        {
            LOG("Animation channel for a node not in the armature");
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
        else
        {
            CHECK_MSG(false, "Channel not supported!");
        }
    }
}

void Model::loadGLTFAnimationFrames(Ptr<Animation> animation)
{
    // https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_007_Animations.md

    FOR_RANGE(frameIt, 0, animation->mDurationInTicks)
    {
        animation->mFrames[frameIt].mTransforms.resize(mBonesIndexCount);
        std::vector<Matrix4> originalFrameTransforms;
        originalFrameTransforms.resize(mBonesIndexCount);

        f32 currentAnimationTime = frameIt*smAnimationFrameRateSeconds;
        FOR_RANGE(boneIt, 0, mBonesIndexCount)
        {
            Vector3 translation(0, 0, 0);
            if (mChannels[boneIt].translate)
            {
                getTranslationAtTime(mChannels[boneIt].translate->sampler->input, mChannels[boneIt].translate->sampler->interpolation, mChannels[boneIt].translate->sampler->output, currentAnimationTime, translation);
            }
            
            Quaternion rotation(0, 0, 0, 1);
            if (mChannels[boneIt].rotate)
            {
                getRotationAtTime(mChannels[boneIt].rotate->sampler->input, mChannels[boneIt].rotate->sampler->interpolation, mChannels[boneIt].rotate->sampler->output, currentAnimationTime, rotation);
            }

            Vector3 scale(1, 1, 1);
            if (mChannels[boneIt].scale)
            {
                getScaleAtTime(mChannels[boneIt].scale->sampler->input, mChannels[boneIt].scale->sampler->interpolation, mChannels[boneIt].scale->sampler->output, currentAnimationTime, scale);
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

            originalFrameTransforms[boneIt] = boneFrameMatrix;
            animation->mFrames[frameIt].mTransforms[boneIt] = boneFrameMatrix;
        }

        FOR_RANGE(i, 0, mBonesIndexCount)
        {
            animation->mFrames[frameIt].mTransforms[i] = calculateHierarchicalBoneTransform(i, originalFrameTransforms);
        }

        FOR_RANGE(boneIt, 0, mBonesIndexCount)
        {
            Matrix4 inverseBindMatrix = mInverseBindMatrices[boneIt];
            Matrix4 boneFrameMatrix = animation->mFrames[frameIt].mTransforms[boneIt];
            boneFrameMatrix.mul(inverseBindMatrix);

            animation->mFrames[frameIt].mTransforms[boneIt] = boneFrameMatrix;
        }
    }
}

void Model::loadGLTFAnimations()
{
    FOR_RANGE(animIt, 0, mCGLTFData->animations_count)
    {
        const cgltf_animation& gltfAnim = mCGLTFData->animations[animIt];
        f32 animDuration = 0.0f;
        animDuration = loadGLTFAnimationDuration(gltfAnim);

        loadGLTFChannels(gltfAnim);

        Ptr<Animation> animation = mAnimations.emplace_back(OwnerPtr<Animation>::newObject());;
        animation->init(animDuration, getPtrToThis<Model>());

        loadGLTFAnimationFrames(animation);

        GET_SYSTEM(AnimationManager).createAnimationState(animation);
    }
}

Matrix4 Model::calculateHierarchicalBoneTransform(u32 boneId, std::vector<Matrix4> originalFrameTransforms) const
{
    Matrix4 boneBindTransform = originalFrameTransforms[boneId];
    if (mBones[boneId].mParentId >= 0)
    {
        Matrix4 parentBoneBindTransform = calculateHierarchicalBoneTransform(mBones[boneId].mParentId, originalFrameTransforms);
        parentBoneBindTransform.mul(boneBindTransform);
        boneBindTransform = parentBoneBindTransform;
    }

    return boneBindTransform;
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

void Model::getTranslationAtTime(cgltf_accessor *input, cgltf_interpolation_type interpolation, cgltf_accessor *output, f32 currentTime, Vector3& out)
{
    KeyframeData keyframeData;
    bool result = findKeyframeData(input, currentTime, keyframeData);
    CHECK_MSG(result, "Error obtaining keyframe data!");
    CHECK_MSG(output->component_type == cgltf_component_type_r_32f, "Component type is not f32!");
    CHECK_MSG(output->type == cgltf_type_vec3, "Type is not vec3!");

    if(interpolation == cgltf_interpolation_type_linear)
    {
        Vector3 v1;
        Vector3 v2;
        cgltf_bool readResult1 = cgltf_accessor_read_float(output, keyframeData.mKeyframe, (cgltf_float*)&v1, 3);
        CHECK_MSG(readResult1, "Couldn't read Translation/Scale data at time: " + std::to_string(currentTime));
        cgltf_bool readResult2 = cgltf_accessor_read_float(output, keyframeData.mKeyframe+1, (cgltf_float*)&v2, 3);
        CHECK_MSG(readResult2, "Couldn't read Translation/Scale data at time: " + std::to_string(currentTime));

        out = v1;
        out.lerp(v2, keyframeData.mInterpolationValue);
    }
    else if(interpolation == cgltf_interpolation_type_step)
    {
        Vector3 v1;
        cgltf_bool readResult1 = cgltf_accessor_read_float(output, keyframeData.mKeyframe, (cgltf_float*)&v1, 3);
        CHECK_MSG(readResult1, "Couldn't read Translation/Scale data at time: " + std::to_string(currentTime));

        out = v1;
    }
    else
    {
        CHECK_MSG(false, "Unsuported Interpolation Type!");
    }
}
void Model::getScaleAtTime(cgltf_accessor *input, cgltf_interpolation_type interpolation, cgltf_accessor *output, f32 currentTime, Vector3& out)
{
    // use same code as Translation
    return getTranslationAtTime(input, interpolation, output, currentTime, out);
}
void Model::getRotationAtTime(cgltf_accessor *input, cgltf_interpolation_type interpolation, cgltf_accessor *output, f32 currentTime, Quaternion& out)
{
    KeyframeData keyframeData;
    bool result = findKeyframeData(input, currentTime, keyframeData);

    CHECK_MSG(result, "Error obtaining keyframe data!");
    CHECK_MSG(output->component_type == cgltf_component_type_r_32f, "Component type is not f32!");
    CHECK_MSG(output->type == cgltf_type_vec4, "Type is not vec4!");
    
    
    if(interpolation == cgltf_interpolation_type_linear)
    {
        Quaternion q1;
        Quaternion q2;
        cgltf_bool readResult1 = cgltf_accessor_read_float(output, keyframeData.mKeyframe, (cgltf_float*)&q1, 4);
        CHECK_MSG(readResult1, "Couldn't read Rotation data at time: " + std::to_string(currentTime));
        cgltf_bool readResult2 = cgltf_accessor_read_float(output, keyframeData.mKeyframe, (cgltf_float*)&q2, 4);
        CHECK_MSG(readResult2, "Couldn't read Rotation data at time: " + std::to_string(currentTime));

        out = q1;
        out.slerp(q2, keyframeData.mInterpolationValue);
    }
    else if(interpolation == cgltf_interpolation_type_step)
    {
        Quaternion q1;
        cgltf_bool readResult1 = cgltf_accessor_read_float(output, keyframeData.mKeyframe, (cgltf_float*)&q1, 4);
        CHECK_MSG(readResult1, "Couldn't read Rotation data at time: " + std::to_string(currentTime));

        out = q1;
    }
    else
    {
        CHECK_MSG(false, "Unsuported Interpolation Type!");
    }
}