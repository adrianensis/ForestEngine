#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/System.hpp"
#include "Graphics/GPU/Mesh/GPUMesh.hpp"

class MeshPrimitives: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

	template <class T>
	Ptr<const GPUMesh> getPrimitive()
	{
		if(!mPrimitivesMap.contains(ClassManager::getClassMetadata<T>().mClassDefinition.getId()))
		{
			mPrimitivesMap.insert_or_assign(ClassManager::getClassMetadata<T>().mClassDefinition.getId(), createPrimitive<T>());
		}
		
		return Ptr<GPUMesh>(mPrimitivesMap.at(ClassManager::getClassMetadata<T>().mClassDefinition.getId()));
	}

private:
	template <class T>
	OwnerPtr<GPUMesh> createPrimitive() const
	{
		CHECK_MSG(false, "GPUMesh Primitive not implemented");
		return OwnerPtr<GPUMesh>();
	}

private:
	std::unordered_map<ClassId, OwnerPtr<GPUMesh>> mPrimitivesMap;
};
REGISTER_CLASS(MeshPrimitives);

template <>
OwnerPtr<GPUMesh> MeshPrimitives::createPrimitive<Rectangle>() const;
template <>
OwnerPtr<GPUMesh> MeshPrimitives::createPrimitive<Cube>() const;
