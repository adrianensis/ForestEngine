#pragma once

// CPP_IGNORE

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

private:
	// Simplifies the batch mapping by texture, mesh and shader
	class BatchKey
	{
	public:
		const Texture* mTexture = nullptr;
		const Shader* mShader = nullptr;
		const Mesh* mMesh = nullptr;

		void init(const Texture* texture, const Shader* shader, const Mesh* mesh)
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

	std::vector<BatchKey> mBatchKeys;

	std::map<BatchKey*, Batch*> mBatchesDynamic;
	std::map<BatchKey*, Batch*> mBatchesDynamicScreenSpace;
	std::map<BatchKey*, Batch*> mBatchesStatic;
	std::map<BatchKey*, Batch*> mBatchesStaticScreenSpace;

	void renderBatchesMap(std::map<BatchKey*, Batch*>& batchesMap);

public:
	~BatchesMap() override;

	void init();
	void render();
	void addRenderer(Renderer& renderer);

	void forceRegenerateBuffers();
};