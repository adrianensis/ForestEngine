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

#ifdef CPP_INCLUDE
template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const
{
	OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>(NEW(Mesh));
	mesh.get().init(4, 2);

	mesh.get().addVertex(Vector3(-0.5f, 0.5f, 0.0f)); // top left
	mesh.get().addVertex(Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
	mesh.get().addVertex(Vector3(0.5f, -0.5f, 0.0f)); // bottom right
	mesh.get().addVertex(Vector3(0.5f, 0.5f, 0.0f)); // top right

	mesh.get().addTexCoord(0.0f, 1.0f); // top left
	mesh.get().addTexCoord(0.0f, 0.0f); // bottom left
	mesh.get().addTexCoord(1.0f, 0.0f); // bottom right
	mesh.get().addTexCoord(1.0f, 1.0f); // top right

	mesh.get().addFace(0, 1, 3);
	mesh.get().addFace(1, 2, 3);

	return mesh;
}

#endif
// template <>
// const Mesh* MeshPrimitives::createPrimitive<Rectangle>() const;