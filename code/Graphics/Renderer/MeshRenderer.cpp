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

    mMeshInstance = OwnerPtr<Mesh>::newObject();
    mMeshInstance->init(mComponentData.mMesh->mVertexCount, mComponentData.mMesh->mFacesCount);
    mMeshInstance->appendToBonesVertexIDsData(mComponentData.mMesh->mBonesVertexIDsData);
    mMeshInstance->appendToBonesVertexWeightsData(mComponentData.mMesh->mBonesVertexWeightsData);

    setColor(Vector4(0, 0, 0, 1));
    mRegeneratePositions = true;
    mRegenerateTextureCoords = true;
}

void MeshRenderer::onComponentAdded() 
{
}

bool MeshRenderer::getIsWorldSpace() const
{
	return mGameObject->mTransform->mAffectedByProjection;
}

void MeshRenderer::calculateRendererModelMatrix()
{
    PROFILER_CPU()
    mRendererModelMatrix.translation(mComponentData.mPositionOffset);
    mRendererModelMatrix.mul(mGameObject->mTransform->getModelMatrix());
}

void MeshRenderer::update()
{
	PROFILER_CPU()

    calculateRendererModelMatrix();

	preUpdate();

    bool regenerateVertices = !mComponentData.mIsInstanced;
    if(regenerateVertices)
    {
        if(mRegeneratePositions)
        {
            mMeshInstance->clearPositions();
            updatePositions();
            mRegeneratePositions = false;
        }
        if(mRegenerateTextureCoords)
        {
            mMeshInstance->clearTextureCoordinates();
            updateTextureCoords();
            mRegenerateTextureCoords = false;
        }
    }

    if(mRegenerateColor && mComponentData.mMaterial and mComponentData.mMaterial->getMaterialData().mUseVertexColor)
    {
        mMeshInstance->setColor(mColor);
        mRegenerateColor = false;
    }
}

void MeshRenderer::updatePositions()
{
    mMeshInstance->appendToPositions(mComponentData.mMesh->mPositions);
}

void MeshRenderer::updateTextureCoords()
{
    mMeshInstance->appendToTextureCoordinates(mComponentData.mMesh->mTextureCoordinates);
}

void MeshRenderer::onDestroy() 
{

}

bool MeshRenderer::hasValidChunk() const
{
	return (! mChunk.isValid()) || (mChunk.isValid() and mChunk->mIsLoaded); // !chunk means -> Screen Space case
}

void MeshRenderer::setMaterial(Ptr<const Material> material)
{
    if(mComponentData.mMaterial != material)
    {
        mComponentData.mMaterial = material;
        if(mBatch)
        {
            mBatch->requestRegenerateBuffers();
        }
    }
}

void MeshRenderer::setColor(const Vector4& color)
{
    if(mColor != color)
    {
        mColor = color;
        mRegenerateColor = true;
        if(mBatch)
        {
            mBatch->requestRegenerateBuffers();
        }
    }
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
	// DESERIALIZE("depth", mComponentData.mDepth)
}