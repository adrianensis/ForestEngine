#pragma once

#include "Core/Module.hpp"
#include "Graphics/Batch/BatchesMap.hpp"
#include "Graphics/Renderer.hpp"

class Chunk: public ObjectBase
{
    GENERATE_METADATA(Chunk)
private:
	std::list<Renderer *> mRenderers;
	Vector3 mLeftTop;
	f32 mSize = 0.0f;
	bool mIsLoaded = false;
	Vector3 mCenter;
	f32 mRadius = 0.0f;

public:
	~Chunk() override;
	void init();
	void set(const Vector3& leftTop, f32 size);
	void load();
	void update(BatchesMap * batchesMap);
	void unload();
	bool containsRenderer(const Renderer *renderer, f32 epsilon = 0.0f) const;
	bool containsRendererSphere(const Renderer *renderer) const;
	void addRenderer(Renderer * renderer);

	GET(IsLoaded)
	GET(Center)
	GET(Radius)
};