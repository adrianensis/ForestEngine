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
		if(!mPrimitivesMap.contains(T::getClassDefinitionStatic().mId))
		{
			mPrimitivesMap.insert_or_assign(T::getClassDefinitionStatic().mId, createPrimitive<T>());
		}
		
		return Ptr<Mesh>(mPrimitivesMap.at(T::getClassDefinitionStatic().mId));
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
