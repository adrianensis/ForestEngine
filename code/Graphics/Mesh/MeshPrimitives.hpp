#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Mesh/MeshPrimitives.hpp"
#endif

class MeshPrimitives: public ObjectBase, public Singleton<MeshPrimitives>
{
	GENERATE_METADATA(MeshPrimitives)

public:
	CPP void init()
	{

	}

	CPP void terminate()
	{
		//MAP_DELETE_CONTENT(mPrimitivesMap);
		mPrimitivesMap.clear();
	}

	template <class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	Ptr<const Mesh> getPrimitive()
	{
		if(!MAP_CONTAINS(mPrimitivesMap, T::getClassIdStatic()))
		{
			OwnerPtr<Mesh> mesh = createPrimitive<T>();

			MAP_INSERT(mPrimitivesMap, T::getClassIdStatic(), mesh);
		}
		
		return Ptr<Mesh>(mPrimitivesMap.at(T::getClassIdStatic()));
	}

private:
	template <class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	OwnerPtr<Mesh> createPrimitive() const
	{
		ASSERT_MSG(false, "Mesh Primitive not implemented");
		return OwnerPtr<Mesh>();
	}

private:
	std::map<ClassId, OwnerPtr<Mesh>> mPrimitivesMap;
};

template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const;
template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Cube>() const;

#ifdef CPP_INCLUDE
template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const
{
	OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>(NEW(Mesh));
	mesh.get().init(4*1, 2*1);

	mesh.get().addVertex(Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
	mesh.get().addVertex(Vector3(0.5f, -0.5f, 0.0f)); // bottom right
	mesh.get().addVertex(Vector3(0.5f, 0.5f, 0.0f)); // top right
	mesh.get().addVertex(Vector3(-0.5f, 0.5f, 0.0f)); // top left

	mesh.get().addTexCoord(0.0f, 0.0f); // bottom left
	mesh.get().addTexCoord(1.0f, 0.0f); // bottom right
	mesh.get().addTexCoord(1.0f, 1.0f); // top right
	mesh.get().addTexCoord(0.0f, 1.0f); // top left

	mesh.get().addFace(0,1,2);
	mesh.get().addFace(2,3,0);

	return mesh;
}

template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Cube>() const
{
	OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>(NEW(Mesh));
	mesh.get().init(4*6, 2*6);

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

	// Front
	mesh.get().addVertex(bottomLeftFront);
	mesh.get().addVertex(bottomRightFront);
	mesh.get().addVertex(topRightFront);
	mesh.get().addVertex(topLeftFront);

	mesh.get().addTexCoord(0.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 1.0f);
	mesh.get().addTexCoord(0.0f, 1.0f);

	mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
	mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

	elementOffset += elementOffsetIncrement;

	// Left
	mesh.get().addVertex(bottomLeftBack);
	mesh.get().addVertex(bottomLeftFront);
	mesh.get().addVertex(topLeftFront);
	mesh.get().addVertex(topLeftBack);

	mesh.get().addTexCoord(0.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 1.0f);
	mesh.get().addTexCoord(0.0f, 1.0f);

	mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
	mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

	elementOffset += elementOffsetIncrement;

	// Back
	mesh.get().addVertex(bottomRightBack);
	mesh.get().addVertex(bottomLeftBack);
	mesh.get().addVertex(topLeftBack);
	mesh.get().addVertex(topRightBack);

	mesh.get().addTexCoord(0.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 1.0f);
	mesh.get().addTexCoord(0.0f, 1.0f);

	mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
	mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

	elementOffset += elementOffsetIncrement;

	// Right
	mesh.get().addVertex(bottomRightFront);
	mesh.get().addVertex(bottomRightBack);
	mesh.get().addVertex(topRightBack);
	mesh.get().addVertex(topRightFront);

	mesh.get().addTexCoord(0.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 1.0f);
	mesh.get().addTexCoord(0.0f, 1.0f);

	mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
	mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

	elementOffset += elementOffsetIncrement;

	// Top
	mesh.get().addVertex(topLeftFront);
	mesh.get().addVertex(topRightFront);
	mesh.get().addVertex(topRightBack);
	mesh.get().addVertex(topLeftBack);

	mesh.get().addTexCoord(0.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 1.0f);
	mesh.get().addTexCoord(0.0f, 1.0f);

	mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
	mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

	elementOffset += elementOffsetIncrement;

	// Bottom
	mesh.get().addVertex(bottomLeftBack);
	mesh.get().addVertex(bottomRightBack);
	mesh.get().addVertex(bottomRightFront);
	mesh.get().addVertex(bottomLeftFront);

	mesh.get().addTexCoord(0.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 0.0f);
	mesh.get().addTexCoord(1.0f, 1.0f);
	mesh.get().addTexCoord(0.0f, 1.0f);

	mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
	mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

	elementOffset += elementOffsetIncrement;

	return mesh;
}
#endif