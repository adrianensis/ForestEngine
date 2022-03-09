#include "Graphics/Batch/BatchesMap.hpp"

#include "Scene/Module.hpp"

BatchesMap::~BatchesMap()
{
	// MAP_DELETE_CONTENT(mBatchesDynamic)
	// MAP_DELETE_CONTENT(mBatchesDynamicScreenSpace)
	// MAP_DELETE_CONTENT(mBatchesStatic)
	// MAP_DELETE_CONTENT(mBatchesStaticScreenSpace)
}

void BatchesMap::init()
{
	//TRACE();
}

void BatchesMap::addRenderer(Ref<Renderer> renderer)
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

	Ref<Transform> transform = renderer.get().getGameObject()->getTransform();

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
		OwnerRef<Batch> batch = OwnerRef<Batch>(NEW(Batch));
		batch.get().init(renderer.get().getMesh(), renderer.get().getMaterial());
		batch.get().setIsStatic(transform.get().isStatic());
		batch.get().setIsWorldSpace(transform.get().getAffectedByProjection());

		MAP_INSERT(*batchesMap, foundBatchKey, batch);
	}

	(*batchesMap).at(foundBatchKey).get().addRenderer(renderer);
}

void BatchesMap::render()
{
	renderBatchesMap(mBatchesStatic);
	renderBatchesMap(mBatchesDynamic);
	renderBatchesMap(mBatchesStaticScreenSpace);
	renderBatchesMap(mBatchesDynamicScreenSpace);
}

void BatchesMap::renderBatchesMap(InternalBatchesMap& batchesMap)
{
	FOR_MAP(it, batchesMap)
	{
		it->second.get().render();
	}
}

void BatchesMap::forceRegenerateBuffers()
{
	FOR_MAP(it, mBatchesStatic)
	{
		it->second.get().forceRegenerateBuffers();
	}
}