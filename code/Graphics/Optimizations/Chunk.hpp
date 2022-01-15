#pragma once

#include "Core/Module.hpp"
#include "Graphics/Optimizations/BatchesMap.hpp"
#include "Graphics/Renderer.hpp"

class Chunk: public ObjectBase
{
    GENERATE_METADATA(Chunk)
	PRI std::list<Renderer *> * mRenderers;
	PRI Vector3 mLeftTop;
	PRI f32 mSize = 0.0f;
	PRI bool mIsLoaded = false; GET(IsLoaded)
	PRI Vector3 mCenter; GET(Center)
	PRI f32 mRadius = 0.0f; GET(Radius)

PUB
	~Chunk() override;
	void init();
	void set(const Vector3& leftTop, f32 size);
	void load();
	void update(BatchesMap * batchesMap);
	void unload();
	bool containsRenderer(const Renderer *renderer, f32 epsilon = 0.0f) const;
	bool containsRendererSphere(const Renderer *renderer) const;
	void addRenderer(Renderer * renderer);
};