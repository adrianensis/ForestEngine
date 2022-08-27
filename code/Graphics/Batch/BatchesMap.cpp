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

	BatchMapByBatchKey* batchesMap = nullptr;

	Ptr<Transform> transform = renderer.get().getGameObject()->getTransform();

	bool isStatic = transform.get().isStatic();
	bool isWorldSpace = transform.get().getAffectedByProjection();
	bool isInstanced = renderer.get().getIsInstanced();
	
	if(isStatic)
	{
		if(isWorldSpace)
		{
			batchesMap = isInstanced ? &mBatchesInstancedStatic : &mBatchesStatic;
		}
		else
		{
			batchesMap = isInstanced ? &mBatchesInstancedStaticScreenSpace : &mBatchesStaticScreenSpace;
		}
	}
	else
	{
		if(isWorldSpace)
		{
			batchesMap = isInstanced ? &mBatchesInstancedDynamic : &mBatchesDynamic;
		}
		else
		{
			batchesMap = isInstanced ? &mBatchesInstancedDynamic : &mBatchesDynamic;
		}
	}

	if (!MAP_CONTAINS(*batchesMap, keyIndex))
	{
		OwnerPtr<Batch> batch = OwnerPtr<Batch>(NEW(Batch));
		batch.get().init(renderer.get().getMesh(), renderer.get().getMaterial(), isStatic, isWorldSpace, isInstanced);

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

	renderBatchesMap(mBatchesInstancedStatic);
	renderBatchesMap(mBatchesInstancedDynamic);
	renderBatchesMap(mBatchesInstancedStaticScreenSpace);
	renderBatchesMap(mBatchesInstancedDynamicScreenSpace);
}

void BatchesMap::forceRegenerateBuffers()
{
	FOR_MAP(it, mBatchesStatic)
	{
		it->second.get().forceRegenerateBuffers();
	}

	FOR_MAP(it, mBatchesStaticScreenSpace)
	{
		it->second.get().forceRegenerateBuffers();
	}

	FOR_MAP(it, mBatchesInstancedStatic)
	{
		it->second.get().forceRegenerateBuffers();
	}

	FOR_MAP(it, mBatchesInstancedStaticScreenSpace)
	{
		it->second.get().forceRegenerateBuffers();
	}
}

void BatchesMap::renderBatchesMap(BatchMapByBatchKey& batchesMap)
{
	PROFILER_CPU()

	FOR_MAP(it, batchesMap)
	{
		it->second.get().render();
	}
}
