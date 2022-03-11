#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class MeshPrimitives: public ObjectBase, public Singleton<MeshPrimitives>
{
	GENERATE_METADATA(MeshPrimitives)

private:
	std::map<ClassId, OwnerRef<Mesh>> mPrimitivesMap;

	template <class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	OwnerRef<Mesh> createPrimitive() const
	{
		ASSERT_MSG(false, "Mesh Primitive not implemented");
		return OwnerRef<Mesh>();
	}
public:
	void init();
	void terminate();

	template <class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	Ref<const Mesh> getPrimitive()
	{
		if(!MAP_CONTAINS(mPrimitivesMap, T::getClassIdStatic()))
		{
			OwnerRef<Mesh> mesh = createPrimitive<T>();

			MAP_INSERT(mPrimitivesMap, T::getClassIdStatic(), mesh);
		}
		
		return Ref<Mesh>(mPrimitivesMap.at(T::getClassIdStatic()));
	}
};

template <>
OwnerRef<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const;

// template <>
// const Mesh* MeshPrimitives::createPrimitive<Rectangle>() const;