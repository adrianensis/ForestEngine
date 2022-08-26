#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Core/Module.hpp"
#include "Graphics/Renderer/Renderer.hpp"


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
    void addRenderer(Ptr<Renderer> renderer);
    bool containsRenderer(Ptr<const Renderer> renderer, f32 epsilon = 0.0f) const;
    bool containsRendererSphere(Ptr<const Renderer> renderer) const;

private:
	std::list<Ptr<Renderer>> mRenderers;
    Cube mCube;
	bool mIsLoaded = false;
	Vector3 mCenter;
	f32 mRadius = 0.0f;
public:
	GET(IsLoaded)
	GET(Center)
	GET(Radius)
};

#endif