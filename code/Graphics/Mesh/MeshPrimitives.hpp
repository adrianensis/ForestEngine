#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class MeshPrimitives: public EngineSystem
{
	GENERATE_METADATA(MeshPrimitives)

public:
    void init();

    void terminate();

	template <class T> T_EXTENDS(T, Shape)
	Ptr<const Mesh> getPrimitive()
	{
		if(!MAP_CONTAINS(mPrimitivesMap, T::getClassIdStatic()))
		{
			MAP_INSERT(mPrimitivesMap, T::getClassIdStatic(), createPrimitive<T>());
		}
		
		return Ptr<Mesh>(mPrimitivesMap.at(T::getClassIdStatic()));
	}

private:
	template <class T> T_EXTENDS(T, Shape)
	SharedPtr<Mesh> createPrimitive() const
	{
		ASSERT_MSG(false, "Mesh Primitive not implemented");
		return SharedPtr<Mesh>();
	}

private:
	std::unordered_map<ClassId, SharedPtr<Mesh>> mPrimitivesMap;
};

template <>
SharedPtr<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const;
template <>
SharedPtr<Mesh> MeshPrimitives::createPrimitive<Cube>() const;
