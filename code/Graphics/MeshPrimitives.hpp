#pragma once

// CPP_IGNORE

#include "Core/Module.hpp"
#include "Graphics/Mesh.hpp"

class MeshPrimitives: public ObjectBase, public Singleton<MeshPrimitives>
{
	GENERATE_METADATA(MeshPrimitives)

private:
	std::map<ClassId, const Mesh*> mPrimitivesMap;

public:
	void init();
	void terminate();

	template <class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	const Mesh* getOrCreatePrimitive()
	{
		if(!MAP_CONTAINS(mPrimitivesMap, T::getClassIdStatic()))
		{
			const Mesh* mesh = createPrimitive<T>();

			MAP_INSERT(mPrimitivesMap, T::getClassIdStatic(), mesh);
		}
		
		return mPrimitivesMap.at(T::getClassIdStatic());
	}

	template <class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	const Mesh* createPrimitive() const
	{
		ASSERT_MSG(false, "Mesh Primitive not implemented");
		return nullptr;
	}
};

template <>
const Mesh* MeshPrimitives::createPrimitive<Rectangle>() const;

// template <>
// const Mesh* MeshPrimitives::createPrimitive<Rectangle>() const;