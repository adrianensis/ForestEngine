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

	bool operator==(const BatchKey& otherBatchKey) const
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
	using BatchMapByBatchKey = std::map<u32, OwnerPtr<Batch>>;

    void renderBatchesMap(BatchMapByBatchKey& batchesMap);

private:

	std::vector<BatchKey> mBatchKeys;

	BatchMapByBatchKey mBatchesStatic;
	BatchMapByBatchKey mBatchesStaticScreenSpace;
	BatchMapByBatchKey mBatchesDynamic;
	BatchMapByBatchKey mBatchesDynamicScreenSpace;
	
	BatchMapByBatchKey mBatchesInstancedStatic;
	BatchMapByBatchKey mBatchesInstancedStaticScreenSpace;
	BatchMapByBatchKey mBatchesInstancedDynamic;
	BatchMapByBatchKey mBatchesInstancedDynamicScreenSpace;
};

#endif