#pragma once

#include "Core/Module.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"


class Chunk: public ObjectBase
{
    GENERATE_METADATA(Chunk)

public:
    ~Chunk() override;

    void init();
    void set(const Vector3& leftTop, f32 size);
    void update();
    void load();
    void unload();
    void addRenderer(Ptr<MeshRenderer> renderer);
    bool containsRenderer(Ptr<const MeshRenderer> renderer, f32 epsilon = 0.0f) const;
    bool containsRendererSphere(Ptr<const MeshRenderer> renderer) const;

private:
	std::list<Ptr<MeshRenderer>> mRenderers;
    Cube mCube;

public:
	bool mIsLoaded = false;
	Vector3 mCenter;
	f32 mRadius = 0.0f;
};
