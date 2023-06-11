#include "Graphics/Renderer/MeshRenderer.hpp"

#include "Graphics/Material/TextureAnimation/TextureAnimationFrame.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Mesh/MeshPrimitives.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Batch/Chunk.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Scene/Module.hpp"

void MeshRenderer::init(RendererData& data) 
{
    ComponentWithData::init(data);

	mColor = (Vector4(0, 0, 0, 1));

    mMeshInstance = OwnerPtr<Mesh>::newObject();
}

void MeshRenderer::onComponentAdded() 
{

}

bool MeshRenderer::getIsWorldSpace() const
{
	return mGameObject->mTransform->mAffectedByProjection;
}

Matrix4 MeshRenderer::getRendererModelMatrix() const
{
    Matrix4 rendererModelMatrix;
    rendererModelMatrix.translation(getComponentData().mPositionOffset);
    rendererModelMatrix.mul(mGameObject->mTransform->getModelMatrix());
    return rendererModelMatrix;
}

void MeshRenderer::update()
{
	PROFILER_CPU()

	preUpdate();

	u32 verticesCount = getComponentData().mMesh->mVertexCount;
    
    bool regenerateVertices = !getComponentData().mIsInstanced;
    if(regenerateVertices)
    {
        mMeshInstance->init(getComponentData().mMesh->mVertexCount, getComponentData().mMesh->mFacesCount);

        updatePositions();
        updateTextureCoords();
                
        mMeshInstance->appendToBonesVertexIDsData(getComponentData().mMesh->mBonesVertexIDsData);
        mMeshInstance->appendToBonesVertexWeightsData(getComponentData().mMesh->mBonesVertexWeightsData);
    }

    mMeshInstance->setColor(mColor);
}

void MeshRenderer::updatePositions()
{
    mMeshInstance->appendToPositions(getComponentData().mMesh->mPositions);
}

void MeshRenderer::updateTextureCoords()
{
    mMeshInstance->appendToTextureCoordinates(getComponentData().mMesh->mTextureCoordinates);
}

void MeshRenderer::onDestroy() 
{

}

bool MeshRenderer::hasValidChunk() const
{
	return (! mChunk.isValid()) || (mChunk.isValid() and mChunk->mIsLoaded); // !chunk means -> Screen Space case
}

IMPLEMENT_SERIALIZATION(MeshRenderer)
{
	Component::serialize(json);

	std::string materialPath = "";

//	if(mMaterial->mTexture)
//	{
//		materialPath = mMaterial->mTexture->mPath;
//	}

	SERIALIZE("material", materialPath)
	// SERIALIZE("region", mTextureRegion)
	// SERIALIZE("depth", mDepth)
}

IMPLEMENT_DESERIALIZATION(MeshRenderer)
{
	std::string materialPath = "";
	DESERIALIZE("material", materialPath)

	//mMaterial = GET_SYSTEM(MaterialManager).loadMaterial(materialPath);

	// DESERIALIZE("region", mTextureRegion)
	// DESERIALIZE("depth", getComponentData().mDepth)
}