#pragma once

#include "Core/Module.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/Shader.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Batch/BatchesMap.hpp"
#include "Scene/Module.hpp"
#endif

// Simplifies the batch mapping by texture, mesh and shader
class BatchKey
{
public:
	Ptr<const Texture> mTexture;
	Ptr<const Shader> mShader;
	Ptr<const Mesh> mMesh;

	void init(Ptr<const Texture> texture, Ptr<const Shader> shader, Ptr<const Mesh> mesh)
	{
		mTexture = texture;
		mShader = shader;
		mMesh = mesh;
	}

	bool operator==(const BatchKey& otherBatchKey) const
	{
		return mTexture == otherBatchKey.mTexture && mShader == otherBatchKey.mShader && mMesh == otherBatchKey.mMesh;
	}
};

class BatchesMap: public ObjectBase
{
    GENERATE_METADATA(BatchesMap)

public:
	CPP ~BatchesMap() override
	{
		// MAP_DELETE_CONTENT(mBatchesDynamic)
		// MAP_DELETE_CONTENT(mBatchesDynamicScreenSpace)
		// MAP_DELETE_CONTENT(mBatchesStatic)
		// MAP_DELETE_CONTENT(mBatchesStaticScreenSpace)
	}

	CPP void init()
	{
		//TRACE();
	}

	CPP void addRenderer(Ptr<Renderer> renderer)
	{
		// Create a temporary key for searching purposes
		BatchKey tmpBatchKey;
		tmpBatchKey.init(
			renderer.get().getMaterial().get().getTexture(), // NOTE : Texture can be nullptr as a valid hash key.
			renderer.get().getMaterial().get().getShader(),
			renderer.get().getMesh()
		);

		// Find if batch key already exists
		BatchKey* foundBatchKey = nullptr;
		FOR_LIST(itBatchKey, mBatchKeys)
		{
			BatchKey* batchKey = &(*itBatchKey);
			foundBatchKey = (*batchKey) == tmpBatchKey ? batchKey : nullptr; 
		}

		// If not found, register the key
		if(!foundBatchKey)
		{
			foundBatchKey = &(mBatchKeys.emplace_back(tmpBatchKey));
			foundBatchKey->init(
				renderer.get().getMaterial().get().getTexture(), // NOTE : Texture can be nullptr as a valid hash key.
				renderer.get().getMaterial().get().getShader(),
				renderer.get().getMesh()
			);
		}

		Ptr<Transform> transform = renderer.get().getGameObject()->getTransform();

		InternalBatchesMap* batchesMap = nullptr;
		
		if(transform.get().isStatic())
		{
			batchesMap = &(transform.get().getAffectedByProjection() ?  mBatchesStatic : mBatchesStaticScreenSpace);
		}
		else
		{
			batchesMap = &(transform.get().getAffectedByProjection() ?  mBatchesDynamic : mBatchesDynamicScreenSpace);
		}

		if (!MAP_CONTAINS(*batchesMap, foundBatchKey))
		{
			OwnerPtr<Batch> batch = OwnerPtr<Batch>(NEW(Batch));
			batch.get().init(renderer.get().getMesh(), renderer.get().getMaterial(),
			transform.get().isStatic(), transform.get().getAffectedByProjection());

			MAP_INSERT(*batchesMap, foundBatchKey, batch);
		}

		(*batchesMap).at(foundBatchKey).get().addRenderer(renderer);
	}

	CPP void render()
	{
		renderBatchesMap(mBatchesStatic);
		renderBatchesMap(mBatchesDynamic);
		renderBatchesMap(mBatchesStaticScreenSpace);
		renderBatchesMap(mBatchesDynamicScreenSpace);
	}

	CPP void forceRegenerateBuffers()
	{
		FOR_MAP(it, mBatchesStatic)
		{
			it->second.get().forceRegenerateBuffers();
		}
	}

private:
	using InternalBatchesMap = std::map<BatchKey*, OwnerPtr<Batch>>;

	CPP void renderBatchesMap(InternalBatchesMap& batchesMap)
	{
		FOR_MAP(it, batchesMap)
		{
			it->second.get().render();
		}
	}

private:

	std::vector<BatchKey> mBatchKeys;

	InternalBatchesMap mBatchesDynamic;
	InternalBatchesMap mBatchesDynamicScreenSpace;
	InternalBatchesMap mBatchesStatic;
	InternalBatchesMap mBatchesStaticScreenSpace;
};