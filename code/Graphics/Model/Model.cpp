#include "Graphics/Model/Model.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h" // Output data structure
#include "assimp/postprocess.h" // Post processing flags

Model::~Model()
{

}

void Model::init(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( path, 
            //aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType);

    if(scene)
    {
        // aiVector3t<f32> assimpPosition;
        // aiVector3t<f32> assimpScale;
        // aiVector3t<f32> assimpRotation;

        // scene->mRootNode->mTransformation.Decompose(assimpScale, assimpRotation, assimpPosition);

        // Vector3 position = Vector3(assimpPosition.x, assimpPosition.y, assimpPosition.z);
        // Vector3 scale = Vector3(assimpScale.x, assimpScale.y, assimpScale.z);
        // Vector3 rotation = Vector3(assimpRotation.x, assimpRotation.y, assimpRotation.z);

        if(scene->HasMeshes())
        {
            FOR_RANGE(meshIt, 0, scene->mNumMeshes)
            {
                aiMesh* assimpMesh = scene->mMeshes[meshIt];

                OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>(NEW(Mesh));
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
                }
            }
        }
    }
}