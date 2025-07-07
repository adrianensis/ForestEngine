#pragma once

#include "Engine/Core.hpp"
#include "GPU/Mesh/GPUMesh.hpp"

class GPUMeshFactory: public System::System
{
public:
    virtual void init() override;
    virtual void terminate() override;

	template <class T>
	Core::WeakPtr<const GPUMesh> getPrimitive()
	{
		if(!mPrimitivesMap.contains(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId()))
		{
			mPrimitivesMap.insert_or_assign(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId(), createPrimitive<T>());
		}
		
		return Core::WeakPtr<GPUMesh>(mPrimitivesMap.at(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId()));
	}

private:
	template <class T>
	Core::OwnerPtr<GPUMesh> createPrimitive() const
	{
		CHECK_MSG(false, "GPUMesh Primitive not implemented");
		return Core::OwnerPtr<GPUMesh>();
	}

private:
	std::unordered_map<Core::ClassId, Core::OwnerPtr<GPUMesh>> mPrimitivesMap;
};
REGISTER_CLASS(GPUMeshFactory);

template <>
Core::OwnerPtr<GPUMesh> GPUMeshFactory::createPrimitive<Maths::Rectangle>() const;
template <>
Core::OwnerPtr<GPUMesh> GPUMeshFactory::createPrimitive<Maths::Cube>() const;
