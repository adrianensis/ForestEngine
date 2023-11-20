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
		if(!mPrimitivesMap.contains(T::getClassIdStatic()))
		{
			MAP_INSERT(mPrimitivesMap, T::getClassIdStatic(), createPrimitive<T>());
		}
		
		return Ptr<Mesh>(mPrimitivesMap.at(T::getClassIdStatic()));
	}

private:
	template <class T> T_EXTENDS(T, Shape)
	OwnerPtr<Mesh> createPrimitive() const
	{
		CHECK_MSG(false, "Mesh Primitive not implemented");
		return OwnerPtr<Mesh>();
	}

private:
	std::unordered_map<ClassId, OwnerPtr<Mesh>> mPrimitivesMap;
};

template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const;
template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Cube>() const;
