#pragma once

#include "Core/Module.hpp"
#include "Core/Systems/System.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class MeshPrimitives: public System
{
	

public:
    void init();

    void terminate();

	template <class T>
	Ptr<const Mesh> getPrimitive()
	{
		if(!mPrimitivesMap.contains(ClassManager::getClassMetadata<T>().mClassDefinition.mId))
		{
			mPrimitivesMap.insert_or_assign(ClassManager::getClassMetadata<T>().mClassDefinition.mId, createPrimitive<T>());
		}
		
		return Ptr<Mesh>(mPrimitivesMap.at(ClassManager::getClassMetadata<T>().mClassDefinition.mId));
	}

private:
	template <class T>
	OwnerPtr<Mesh> createPrimitive() const
	{
		CHECK_MSG(false, "Mesh Primitive not implemented");
		return OwnerPtr<Mesh>();
	}

private:
	std::unordered_map<ClassId, OwnerPtr<Mesh>> mPrimitivesMap;
};
REGISTER_CLASS(MeshPrimitives);

template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const;
template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Cube>() const;
