#include "GPU/Mesh/GPUMeshFactory.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"

void GPUMeshFactory::init()
{

}

void GPUMeshFactory::terminate()
{
	mPrimitivesMap.clear();
}

template <>
Core::OwnerPtr<GPUMesh> GPUMeshFactory::createPrimitive<Maths::Rectangle>() const
{
    std::vector<GPUVariableData> gpuVertexInputBuffers;
    gpuVertexInputBuffers.push_back(GPUShaderDefinitions::VertexInput::mPosition);
    gpuVertexInputBuffers.push_back(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0));
    gpuVertexInputBuffers.push_back(GPUShaderDefinitions::VertexInput::mNormal);

	Core::OwnerPtr<GPUMesh> mesh = Core::OwnerPtr<GPUMesh>::newObject();
	mesh->init(4*1, 2*1, gpuVertexInputBuffers);

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(Maths::Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(Maths::Vector3(0.5f, -0.5f, 0.0f)); // bottom right
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(Maths::Vector3(0.5f, 0.5f, 0.0f)); // top right
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(Maths::Vector3(-0.5f, 0.5f, 0.0f)); // top left

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 1.0f)); // bottom left
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 1.0f)); // bottom right
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 0.0f)); // top right
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 0.0f)); // top left

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(Maths::Vector3(0,0,0)); // top
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(Maths::Vector3(0,0,0)); // right
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(Maths::Vector3(0,0,0)); // bottom
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(Maths::Vector3(0,0,0)); // left

	mesh->mIndices.pushBack(Maths::Face(0,1,2));
	mesh->mIndices.pushBack(Maths::Face(2,3,0));

    mesh->mMin = mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).get<Maths::Vector3>(0);
    mesh->mMax = mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).get<Maths::Vector3>(2);

	return mesh;
}

template <>
Core::OwnerPtr<GPUMesh> GPUMeshFactory::createPrimitive<Maths::Cube>() const
{
    std::vector<GPUVariableData> gpuVertexInputBuffers;
    gpuVertexInputBuffers.push_back(GPUShaderDefinitions::VertexInput::mPosition);
    gpuVertexInputBuffers.push_back(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0));
    gpuVertexInputBuffers.push_back(GPUShaderDefinitions::VertexInput::mNormal);

	Core::OwnerPtr<GPUMesh> mesh = Core::OwnerPtr<GPUMesh>::newObject();
	mesh->init(4*6, 2*6, gpuVertexInputBuffers);

	Core::u32 elementOffsetIncrement = 4;
	Core::u32 elementOffset = 0;

	Maths::Vector3 bottomLeftFront(-0.5f, -0.5f, 0.5f); // bottom left front
	Maths::Vector3 bottomRightFront(0.5f, -0.5f, 0.5f); // bottom right front
	Maths::Vector3 topLeftFront(-0.5f, 0.5f, 0.5f); // top right front
	Maths::Vector3 topRightFront(0.5f, 0.5f, 0.5f); // top left front

	Maths::Vector3 bottomLeftBack(-0.5f, -0.5f, -0.5f); // bottom left back
	Maths::Vector3 bottomRightBack(0.5f, -0.5f, -0.5f); // bottom right back
	Maths::Vector3 topLeftBack(-0.5f, 0.5f, -0.5f); // top right back
	Maths::Vector3 topRightBack(0.5f, 0.5f, -0.5f); // top left back

	Maths::Vector3 leftNormal(-1,0,0); // left
	Maths::Vector3 rightNormal(1,0,0); // right
	Maths::Vector3 frontNormal(0,0,1); // front
	Maths::Vector3 backNormal(0,0,-1); // back
	Maths::Vector3 topNormal(0,1,0); // top
	Maths::Vector3 bottomNormal(0,-1,0); // bottom

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomLeftFront);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomRightFront);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topRightFront);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topLeftFront);
	
    mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(frontNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(frontNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(frontNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(frontNormal);

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 0.0f));

	mesh->mIndices.pushBack(Maths::Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Maths::Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomLeftBack);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomLeftFront);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topLeftFront);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topLeftBack);
	
    mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(leftNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(leftNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(leftNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(leftNormal);

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 0.0f));

	mesh->mIndices.pushBack(Maths::Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Maths::Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomRightBack);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomLeftBack);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topLeftBack);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topRightBack);
	
    mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(backNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(backNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(backNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(backNormal);

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 0.0f));

	mesh->mIndices.pushBack(Maths::Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Maths::Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomRightFront);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomRightBack);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topRightBack);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topRightFront);
	
    mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(rightNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(rightNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(rightNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(rightNormal);

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 0.0f));

	mesh->mIndices.pushBack(Maths::Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Maths::Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topLeftFront);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topRightFront);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topRightBack);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(topLeftBack);
	
    mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(topNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(topNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(topNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(topNormal);

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 0.0f));

	mesh->mIndices.pushBack(Maths::Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Maths::Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomLeftBack);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomRightBack);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomRightFront);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mPosition.mName).pushBack(bottomLeftFront);
	
    mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(bottomNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(bottomNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(bottomNormal);
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mNormal.mName).pushBack(bottomNormal);

	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 1.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(1.0f, 0.0f));
	mesh->mBuffers.at(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0).mName).pushBack(Maths::Vector2(0.0f, 0.0f));

	mesh->mIndices.pushBack(Maths::Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mIndices.pushBack(Maths::Face(2+elementOffset,3+elementOffset,0+elementOffset));

    mesh->mMin = bottomLeftBack;
    mesh->mMax = topRightFront;

	return mesh;
}

