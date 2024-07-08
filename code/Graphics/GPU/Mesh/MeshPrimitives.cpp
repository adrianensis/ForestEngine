#include "Graphics/GPU/Mesh/MeshPrimitives.hpp"


void MeshPrimitives::init()
{

}

void MeshPrimitives::terminate()
{
	mPrimitivesMap.clear();
}

template <>
OwnerPtr<GPUMesh> MeshPrimitives::createPrimitive<Rectangle>() const
{
    std::vector<GPUVariableData> gpuVertexInputBuffers;
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mPosition);
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mTextureCoords.at(0));
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mNormal);

	OwnerPtr<GPUMesh> mesh = OwnerPtr<GPUMesh>::newObject();
	mesh->init(4*1, 2*1, gpuVertexInputBuffers);

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(Vector3(0.5f, -0.5f, 0.0f)); // bottom right
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(Vector3(0.5f, 0.5f, 0.0f)); // top right
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(Vector3(-0.5f, 0.5f, 0.0f)); // top left

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 0.0f)); // bottom left
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 0.0f)); // bottom right
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 1.0f)); // top right
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 1.0f)); // top left

	mesh->mIndices.pushBack(Face(0,1,2));
	mesh->mIndices.pushBack(Face(2,3,0));

    mesh->mMin = mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).get<Vector3>(0);
    mesh->mMax = mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).get<Vector3>(2);

	return mesh;
}

template <>
OwnerPtr<GPUMesh> MeshPrimitives::createPrimitive<Cube>() const
{
    std::vector<GPUVariableData> gpuVertexInputBuffers;
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mPosition);
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mTextureCoords.at(0));
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mNormal);

	OwnerPtr<GPUMesh> mesh = OwnerPtr<GPUMesh>::newObject();
	mesh->init(4*6, 2*6, gpuVertexInputBuffers);

	u32 elementOffsetIncrement = 4;
	u32 elementOffset = 0;

	Vector3 bottomLeftFront(-0.5f, -0.5f, 0.5f); // bottom left front
	Vector3 bottomRightFront(0.5f, -0.5f, 0.5f); // bottom right front
	Vector3 topLeftFront(-0.5f, 0.5f, 0.5f); // top right front
	Vector3 topRightFront(0.5f, 0.5f, 0.5f); // top left front

	Vector3 bottomLeftBack(-0.5f, -0.5f, -0.5f); // bottom left back
	Vector3 bottomRightBack(0.5f, -0.5f, -0.5f); // bottom right back
	Vector3 topLeftBack(-0.5f, 0.5f, -0.5f); // top right back
	Vector3 topRightBack(0.5f, 0.5f, -0.5f); // top left back

	Vector3 leftNormal(-1,0,0); // left
	Vector3 rightNormal(1,0,0); // right
	Vector3 frontNormal(0,0,1); // front
	Vector3 backNormal(0,0,-1); // back
	Vector3 topNormal(0,1,0); // top
	Vector3 bottomNormal(0,-1,0); // bottom

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomLeftFront);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomRightFront);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topRightFront);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topLeftFront);
	
    mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(frontNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(frontNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(frontNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(frontNormal);

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 1.0f));

	mesh->mIndices.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomLeftBack);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomLeftFront);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topLeftFront);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topLeftBack);
	
    mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(leftNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(leftNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(leftNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(leftNormal);

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 1.0f));

	mesh->mIndices.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomRightBack);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomLeftBack);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topLeftBack);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topRightBack);
	
    mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(backNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(backNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(backNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(backNormal);

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 1.0f));

	mesh->mIndices.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomRightFront);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomRightBack);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topRightBack);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topRightFront);
	
    mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(rightNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(rightNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(rightNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(rightNormal);

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 1.0f));

	mesh->mIndices.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topLeftFront);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topRightFront);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topRightBack);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(topLeftBack);
	
    mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(topNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(topNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(topNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(topNormal);

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 1.0f));

	mesh->mIndices.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomLeftBack);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomRightBack);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomRightFront);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName).pushBack(bottomLeftFront);
	
    mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(bottomNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(bottomNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(bottomNormal);
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mNormal.mName).pushBack(bottomNormal);

	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoords.at(0).mName).pushBack(Vector2(0.0f, 1.0f));

	mesh->mIndices.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

    mesh->mMin = bottomLeftBack;
    mesh->mMax = topRightFront;

	return mesh;
}

