#pragma once

#include "Core/CoreBase.hpp"
#include "Core/Memory/Singleton.hpp"
#include "GPU/Mesh/GPUMesh.hpp"

class MeshFactory: public Core::Singleton<MeshFactory>
{
public:
	template <class T>
	Core::WeakPtr<const GPUMesh> getPrimitive()
	{
		Core::HashValue id = getPrimitiveId<T>();
		if(!mPrimitivesMap.contains(id))
		{
			mPrimitivesMap.insert_or_assign(id, createPrimitive<T>());
		}
		
		return Core::WeakPtr<GPUMesh>(mPrimitivesMap.at(id));
	}

private:
	template <class T>
	Core::OwnerPtr<GPUMesh> createPrimitive() const
	{
		CHECK_MSG(false, "GPUMesh Primitive not implemented");
		return Core::OwnerPtr<GPUMesh>();
	}

	template <class T>
	Core::HashValue getPrimitiveId() const
	{
		CHECK_MSG(false, "GPUMesh Primitive not implemented");
		return 0;
	}

private:
	std::unordered_map<Core::ClassId, Core::OwnerPtr<GPUMesh>> mPrimitivesMap;
};

template <>
Core::OwnerPtr<GPUMesh> MeshFactory::createPrimitive<Maths::Rectangle>() const;
template <>
Core::HashValue MeshFactory::getPrimitiveId<Maths::Rectangle>() const;
template <>
Core::OwnerPtr<GPUMesh> MeshFactory::createPrimitive<Maths::Cube>() const;
template <>
Core::HashValue MeshFactory::getPrimitiveId<Maths::Cube>() const;