#include "Graphics/Batch/BatchesMap.hpp"
#include "Scene/Module.hpp"


BatchesMap::~BatchesMap() 
{
}

void BatchesMap::init()
{
}

void BatchesMap::addRenderer(Ptr<Renderer> renderer)
{
	BatchKey tmpBatchKey;
	tmpBatchKey.init(
		renderer.get().getMaterial().get().getTexture(), // NOTE : Texture can be nullptr as a valid hash key.
		renderer.get().getMaterial().get().getShader(),
		renderer.get().getMesh()
	);

	// BatchKey* foundBatchKey = nullptr;
	// FOR_LIST_COND(itBatchKey, mBatchKeys, foundBatchKey == nullptr)
	// {
	// 	BatchKey* batchKey = &(*itBatchKey);
	// 	foundBatchKey = (*batchKey) == tmpBatchKey ? batchKey : nullptr;
	// }

	bool keyFound = false;
	u32 keyIndex = 0;
	FOR_RANGE_COND(i, 0, mBatchKeys.size(), !keyFound)
	{
		BatchKey& batchKey = mBatchKeys[i];

		keyFound = batchKey == tmpBatchKey;

		if(keyFound)
		{
			keyIndex = i;
		}
	}

	if(!keyFound)
	{
		mBatchKeys.push_back(tmpBatchKey);
		//foundBatchKey = &(mBatchKeys.emplace_back(tmpBatchKey));

		keyIndex = mBatchKeys.size() - 1;
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

	if (!MAP_CONTAINS(*batchesMap, keyIndex))
	{
		OwnerPtr<Batch> batch = OwnerPtr<Batch>(NEW(Batch));
		batch.get().init(renderer.get().getMesh(), renderer.get().getMaterial(),
		transform.get().isStatic(), transform.get().getAffectedByProjection());

		MAP_INSERT(*batchesMap, keyIndex, batch);
	}

	(*batchesMap).at(keyIndex).get().addRenderer(renderer);
}

void BatchesMap::render()
{
	PROFILER_CPU()

	renderBatchesMap(mBatchesStatic);
	renderBatchesMap(mBatchesDynamic);
	renderBatchesMap(mBatchesStaticScreenSpace);
	renderBatchesMap(mBatchesDynamicScreenSpace);
}

void BatchesMap::forceRegenerateBuffers()
{
	FOR_MAP(it, mBatchesStatic)
	{
		it->second.get().forceRegenerateBuffers();
	}
}

void BatchesMap::renderBatchesMap(InternalBatchesMap& batchesMap)
{
	PROFILER_CPU()

	FOR_MAP(it, batchesMap)
	{
		it->second.get().render();
	}
}
