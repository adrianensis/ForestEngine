#pragma once

#include "Core/Module.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Optimizations/Batch.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Material/Shader.hpp"

class BatchesMap: public ObjectBase
{
    GENERATE_METADATA(BatchesMap)

	// Simplifies the batch mapping by texture, mesh and shader
	PRI class BatchKey
	{
		PUB const Texture* mTexture = nullptr;
		PUB const Shader* mShader = nullptr;
		PUB const Mesh* mMesh = nullptr;

		PUB void init(const Texture* texture, const Shader* shader, const Mesh* mesh)
		{
			mTexture = texture;
			mShader = shader;
			mMesh = mesh;
		}

		PUB bool operator==(const BatchKey& otherBatchKey) const
		{
			return mTexture == otherBatchKey.mTexture && mShader == otherBatchKey.mShader && mMesh == otherBatchKey.mMesh;
		}
	};

	PRI std::vector<BatchKey> mBatchKeys;

	PRI std::map<BatchKey*, Batch*> mBatchesDynamic;
	PRI std::map<BatchKey*, Batch*> mBatchesDynamicScreenSpace;
	PRI std::map<BatchKey*, Batch*> mBatchesStatic;
	PRI std::map<BatchKey*, Batch*> mBatchesStaticScreenSpace;

	void renderBatchesMap(std::map<BatchKey*, Batch*>& batchesMap);

PUB
	~BatchesMap() override;

	void init();
	void render();
	void addRenderer(Renderer& renderer);

	void forceRegenerateBuffers();
};