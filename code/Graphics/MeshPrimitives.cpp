#include "Graphics/MeshPrimitives.hpp"

void MeshPrimitives::init()
{

}

void MeshPrimitives::terminate()
{
	MAP_DELETE_CONTENT(mPrimitivesMap);
}

template <>
const Mesh* MeshPrimitives::createPrimitive<Rectangle>() const
{
	Mesh* mesh = NEW(Mesh);
	mesh->init(4, 2);

	mesh->addVertex(Vector3(-0.5f, 0.5f, 0.0f)); // top left
	mesh->addVertex(Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
	mesh->addVertex(Vector3(0.5f, -0.5f, 0.0f)); // bottom right
	mesh->addVertex(Vector3(0.5f, 0.5f, 0.0f)); // top right

	mesh->addTexCoord(0.0f, 1.0f); // top left
	mesh->addTexCoord(0.0f, 0.0f); // bottom left
	mesh->addTexCoord(1.0f, 0.0f); // bottom right
	mesh->addTexCoord(1.0f, 1.0f); // top right

	mesh->addFace(0, 1, 3);
	mesh->addFace(1, 2, 3);

	return mesh;
}

// template <>
// const Mesh* MeshPrimitives::createPrimitive<Rectangle>() const
// {
// 	Mesh* mesh = NEW(Mesh);
// 	mesh->init(4, 2);

// 	mesh->
// 	// front
// 	addVertex(Vector3(-0.5f, 0.5f, -0.5f))-> // top left
// 	addVertex(Vector3(-0.5f, -0.5f, -0.5f))-> // bottom left
// 	addVertex(Vector3(0.5f, -0.5f, -0.5f))-> // bottom right
// 	addVertex(Vector3(0.5f, 0.5f, -0.5f))-> // top right

// 	// back
// 	addVertex(Vector3(-0.5f, 0.5f, 0.5f))-> // top left
// 	addVertex(Vector3(-0.5f, -0.5f, 0.5f))-> // bottom left
// 	addVertex(Vector3(0.5f, -0.5f, 0.5f))-> // bottom right
// 	addVertex(Vector3(0.5f, 0.5f, 0.5f))-> // top right

// 	// front
// 	addTexCoord(0.0f, 1.0f)-> // top left
// 	addTexCoord(0.0f, 0.0f)-> // bottom left
// 	addTexCoord(1.0f, 0.0f)-> // bottom right
// 	addTexCoord(1.0f, 1.0f)-> // top right

// 	// back
// 	addTexCoord(0.0f, 1.0f)-> // top left
// 	addTexCoord(0.0f, 0.0f)-> // bottom left
// 	addTexCoord(1.0f, 0.0f)-> // bottom right
// 	addTexCoord(1.0f, 1.0f)-> // top right

// 	// top
// 	addTexCoord(0.0f, 1.0f)-> // top left
// 	addTexCoord(0.0f, 0.0f)-> // bottom left
// 	addTexCoord(1.0f, 0.0f)-> // bottom right
// 	addTexCoord(1.0f, 1.0f)-> // top right

// 	// bottom
// 	addTexCoord(0.0f, 1.0f)-> // top left
// 	addTexCoord(0.0f, 0.0f)-> // bottom left
// 	addTexCoord(1.0f, 0.0f)-> // bottom right
// 	addTexCoord(1.0f, 1.0f)-> // top right

// 	// right
// 	addTexCoord(0.0f, 1.0f)-> // top left
// 	addTexCoord(0.0f, 0.0f)-> // bottom left
// 	addTexCoord(1.0f, 0.0f)-> // bottom right
// 	addTexCoord(1.0f, 1.0f)-> // top right

// 	// left
// 	addTexCoord(0.0f, 1.0f)-> // top left
// 	addTexCoord(0.0f, 0.0f)-> // bottom left
// 	addTexCoord(1.0f, 0.0f)-> // bottom right
// 	addTexCoord(1.0f, 1.0f)-> // top right

// 	// front
// 	addFace(0, 1, 3)->
// 	addFace(1, 2, 3);

// 	return mesh;
// }