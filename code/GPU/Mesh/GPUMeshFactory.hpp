#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "GPU/Mesh/GPUMesh.hpp"

class GPUMeshFactory: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

	template <class T>
	WeakPtr<const GPUMesh> getPrimitive()
	{
		if(!mPrimitivesMap.contains(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId()))
		{
			mPrimitivesMap.insert_or_assign(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId(), createPrimitive<T>());
		}
		
		return WeakPtr<GPUMesh>(mPrimitivesMap.at(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId()));
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
REGISTER_CLASS(GPUMeshFactory);

template <>
OwnerPtr<GPUMesh> GPUMeshFactory::createPrimitive<Rectangle>() const;
template <>
OwnerPtr<GPUMesh> GPUMeshFactory::createPrimitive<Cube>() const;
