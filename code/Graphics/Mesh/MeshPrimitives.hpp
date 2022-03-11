#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class MeshPrimitives: public ObjectBase, public Singleton<MeshPrimitives>
{
	GENERATE_METADATA(MeshPrimitives)

private:
	std::map<ClassId, OwnerPtr<Mesh>> mPrimitivesMap;

	template <class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	OwnerPtr<Mesh> createPrimitive() const
	{
		ASSERT_MSG(false, "Mesh Primitive not implemented");
		return OwnerPtr<Mesh>();
	}
public:
	void init();
	void terminate();

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
};

template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const;

// template <>
// const Mesh* MeshPrimitives::createPrimitive<Rectangle>() const;