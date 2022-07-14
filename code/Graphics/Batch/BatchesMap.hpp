#ifndef BATCHESMAP_HPP
#define BATCHESMAP_HPP

#include "Core/Module.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/Shader.hpp"


class BatchKey
{
public:
	BatchKey() = default;
	
	Ptr<const Texture> mTexture;
	Ptr<const Shader> mShader;
	Ptr<const Mesh> mMesh;

	void init(Ptr<const Texture> texture, Ptr<const Shader> shader, Ptr<const Mesh> mesh)
	{
		mTexture = texture;
		mShader = shader;
		mMesh = mesh;
	}

	bool operator==(CR(BatchKey) otherBatchKey) const
	{
		return mTexture == otherBatchKey.mTexture && mShader == otherBatchKey.mShader && mMesh == otherBatchKey.mMesh;
	}
};

class BatchesMap: public ObjectBase
{
    GENERATE_METADATA(BatchesMap)

public:
    ~BatchesMap() override;

    void init();
    void addRenderer(Ptr<Renderer> renderer);
    void render();
    void forceRegenerateBuffers();

private:
	using InternalBatchesMap = std::map<u32, OwnerPtr<Batch>>;
    void renderBatchesMap(R(InternalBatchesMap) batchesMap);

private:

	std::vector<BatchKey> mBatchKeys;

	InternalBatchesMap mBatchesDynamic;
	InternalBatchesMap mBatchesDynamicScreenSpace;
	InternalBatchesMap mBatchesStatic;
	InternalBatchesMap mBatchesStaticScreenSpace;
};

#endif