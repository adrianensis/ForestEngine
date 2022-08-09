#include "Graphics/Model/Model.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "assimp/scene.h" // Output data structure
#include "assimp/postprocess.h" // Post processing flags

Model::~Model()
{

}

void Model::init(CR(std::string) path)
{
    const aiScene* scene = mImporter.ReadFile( path, 
            //aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType);

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

        if(scene->HasMeshes())
        {
            FOR_RANGE(meshIt, 0, scene->mNumMeshes)
            {
                aiMesh* assimpMesh = scene->mMeshes[meshIt];

                OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>(NEW(Mesh));
                mesh.get().setModel(getPtrToThis());
                mMeshes.push_back(mesh);

                mesh.get().init(assimpMesh->mNumVertices, assimpMesh->mNumFaces);

                if(assimpMesh->HasTextureCoords(0))
                {
                    FOR_RANGE(vertexIt, 0, assimpMesh->mNumVertices)
                    {
                        aiVector3D assimpVertex = assimpMesh->mVertices[vertexIt];
                        Vector3 vertex = Vector3(assimpVertex.x, assimpVertex.y, assimpVertex.z);
                        mesh.get().addVertex(vertex);

                        aiVector3D assimpTexCoord = assimpMesh->mTextureCoords[0][vertexIt];

                        Vector2 texCoord = Vector2(assimpTexCoord.x, assimpTexCoord.y);
                        mesh.get().addTexCoord(texCoord.x, texCoord.y);

                        mesh.get().addBoneVertexData(BoneVertexData());
                    }
                }

                FOR_RANGE(faceIt, 0, assimpMesh->mNumFaces)
                {
                    aiFace assimpFace = assimpMesh->mFaces[faceIt];
                    mesh.get().addFace(assimpFace.mIndices[0], assimpFace.mIndices[1], assimpFace.mIndices[2]);
                }

                if(scene->HasMaterials())
                {
                    aiMaterial* material = scene->mMaterials[assimpMesh->mMaterialIndex];
                    
                    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
                    {
                        aiString materialPath;

                        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &materialPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
                        {
                            mesh.get().setMaterialPath(materialPath.data);
                        }
                    }

                    // TODO : REMOVE - TEST
                    mesh.get().setMaterialPath("resources/bob_lamp/bob_body.png");
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
                            ECHO("Registering")

                            BoneData boneData;
                            boneData.mId = mBonesIndexCount;

                            CR(aiMatrix4x4) assimpBoneOffsetMatrix = currentBone->mOffsetMatrix;
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
                            //assert(vertexId <= vertices.size());
                            mesh.get().addBoneWeight(vertexIndex, boneIndex, weight);
			            }
                    }
                }
            }
        }

        if(scene->HasAnimations())
        {
            // FOR_RANGE(animIt, 0, scene->mNumAnimations)
            // {
            //     const aiAnimation* animation = scene->mAnimations[animIt];

            //     std::string nodeName(pNode->mName.data);

            //     const aiNodeAnim* pNodeAnim = findNodeAnim(animation, nodeName);
            // }
        }
    }
    else
    {
       printf("Error parsing '%s': '%s'\n", path.c_str(), mImporter.GetErrorString());
    }
}

