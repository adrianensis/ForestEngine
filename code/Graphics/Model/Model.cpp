#include "Graphics/Model/Model.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Animation/Animation.hpp"

#include "assimp/scene.h" // Output data structure
#include "assimp/postprocess.h" // Post processing flags

Model::~Model()
{

}

void Model::init(const std::string& path)
{
    mPath = Paths::mResources + path;

    const aiScene* scene = mImporter.ReadFile( mPath,
            //aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_SortByPType);

    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        printf("Error parsing '%s': '%s'\n", path.c_str(), mImporter.GetErrorString());
        return;
    }

    if(scene)
    {
        aiMatrix4x4 globalInverseTransform = scene->mRootNode->mTransformation;
        mGlobalInverseTransform.init(
            Vector4(globalInverseTransform.a1, globalInverseTransform.a2, globalInverseTransform.a3, globalInverseTransform.a4),
            Vector4(globalInverseTransform.b1, globalInverseTransform.b2, globalInverseTransform.b3, globalInverseTransform.b4),
            Vector4(globalInverseTransform.c1, globalInverseTransform.c2, globalInverseTransform.c3, globalInverseTransform.c4),
            Vector4(globalInverseTransform.d1, globalInverseTransform.d2, globalInverseTransform.d3, globalInverseTransform.d4)
        );
        mGlobalInverseTransform.invert();

        // relationship map: assimp material ID -> own material ID
        std::vector<Ptr<Material>> assimpMaterials;

        if(scene->HasMaterials())
        {
            assimpMaterials.resize(scene->mNumMaterials);

            FOR_RANGE(materialIt, 0, scene->mNumMaterials)
            {
                aiMaterial* material = scene->mMaterials[materialIt];
                Ptr<Material> newMaterial;
                if(! assimpMaterials[materialIt].isValid())
                {
                    newMaterial = GET_SYSTEM(MaterialManager).createMaterial();
                    assimpMaterials[materialIt] = newMaterial;
                }

                FOR_RANGE(itTexture, 0, TextureType::MAX)
                {
                    aiTextureType assimpTextureType = static_cast<aiTextureType>(itTexture);
                    TextureType textureType = static_cast<TextureType>(itTexture);
                    if (material->GetTextureCount(assimpTextureType) > 0)
                    {
                        aiString materialPath;
                        if (material->GetTexture(assimpTextureType, 0, &materialPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
                        {
                            std::filesystem::path texturePath = mPath.parent_path().append(materialPath.data);
                            newMaterial.get().mTextures[(u32)textureType] = GET_SYSTEM(MaterialManager).loadTexture(texturePath);
                        }
                    }
                }
            }
        }

        if(scene->HasAnimations())
        {
            FOR_RANGE(animIt, 0, scene->mNumAnimations)
            {
                OwnerPtr<Animation> animation = OwnerPtr<Animation>::newObject();
                animation.get().init(animIt, getPtrToThis());
                mAnimations.emplace_back(animation);

                GET_SYSTEM(AnimationManager).createAnimationState(animation);
            }
        }

        if(scene->HasMeshes())
        {
            FOR_RANGE(meshIt, 0, scene->mNumMeshes)
            {
                aiMesh* assimpMesh = scene->mMeshes[meshIt];

                OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>::newObject();
                mesh.get().mModel = (getPtrToThis());
                mMeshes.push_back(mesh);

                mesh.get().init(assimpMesh->mNumVertices, assimpMesh->mNumFaces);
                mesh.get().setColor(Vector4(0,0,0,1));

                mesh.get().mMaterial = assimpMaterials[assimpMesh->mMaterialIndex];

                if(assimpMesh->HasTextureCoords(0))
                {
                    FOR_RANGE(vertexIt, 0, assimpMesh->mNumVertices)
                    {
                        aiVector3D assimpVertex = assimpMesh->mVertices[vertexIt];
                        Vector3 position = Vector3(assimpVertex.x, assimpVertex.y, assimpVertex.z);
                        mesh.get().addToPositions(position);

                        aiVector3D assimpTexCoord = assimpMesh->mTextureCoords[0][vertexIt];

                        Vector2 texCoord = Vector2(assimpTexCoord.x, assimpTexCoord.y);
                        mesh.get().addToTextureCoordinates(texCoord);

                        mesh.get().addToBonesVertexIDsData(BoneVertexIDsData());
                        mesh.get().addToBonesVertexWeightsData(BoneVertexWeightsData());
                    }
                }

                FOR_RANGE(faceIt, 0, assimpMesh->mNumFaces)
                {
                    const aiFace& assimpFace = assimpMesh->mFaces[faceIt];
                    ASSERT_MSG(assimpFace.mNumIndices == 3, "Face num indices != 3")
                    mesh.get().addToFaces(Face(assimpFace.mIndices[0], assimpFace.mIndices[1], assimpFace.mIndices[2]));
                }

                if(assimpMesh->HasBones())
                {
                    FOR_RANGE(boneIt, 0, assimpMesh->mNumBones)
                    {
                        u32 boneIndex = 0;

                        aiBone* currentBone = assimpMesh->mBones[boneIt];
                        std::string boneName(currentBone->mName.data);

                        if (! MAP_CONTAINS(mBonesMapping, boneName)) 
                        {
                            BoneData boneData;
                            boneData.mId = mBonesIndexCount;

                            const aiMatrix4x4& assimpBoneOffsetMatrix = currentBone->mOffsetMatrix;
                            boneData.mOffsetMatrix.init(
                                Vector4(assimpBoneOffsetMatrix.a1, assimpBoneOffsetMatrix.a2, assimpBoneOffsetMatrix.a3, assimpBoneOffsetMatrix.a4),
                                Vector4(assimpBoneOffsetMatrix.b1, assimpBoneOffsetMatrix.b2, assimpBoneOffsetMatrix.b3, assimpBoneOffsetMatrix.b4),
                                Vector4(assimpBoneOffsetMatrix.c1, assimpBoneOffsetMatrix.c2, assimpBoneOffsetMatrix.c3, assimpBoneOffsetMatrix.c4),
                                Vector4(assimpBoneOffsetMatrix.d1, assimpBoneOffsetMatrix.d2, assimpBoneOffsetMatrix.d3, assimpBoneOffsetMatrix.d4)
                            );

                            MAP_INSERT(mBonesMapping, boneName, boneData);

                            mBonesIndexCount++;
                        }

                        boneIndex = mBonesMapping.at(boneName).mId;

                        aiVertexWeight* weights = currentBone->mWeights;
                        FOR_RANGE(weightIt, 0, currentBone->mNumWeights)
                        {
                            u32 vertexIndex = weights[weightIt].mVertexId;
                            f32 weight = weights[weightIt].mWeight;
                            mesh.get().addBoneWeight(vertexIndex, boneIndex, weight);
			            }
                    }
                }
            }
        }
    }
}