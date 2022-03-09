#pragma once

#include "Core/Module.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Batch/Batch.hpp"
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
		Ref<const Texture> mTexture;
		Ref<const Shader> mShader;
		Ref<const Mesh> mMesh;

		void init(Ref<const Texture> texture, Ref<const Shader> shader, Ref<const Mesh> mesh)
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

	using InternalBatchesMap = std::map<BatchKey*, OwnerRef<Batch>>;
	InternalBatchesMap mBatchesDynamic;
	InternalBatchesMap mBatchesDynamicScreenSpace;
	InternalBatchesMap mBatchesStatic;
	InternalBatchesMap mBatchesStaticScreenSpace;

	void renderBatchesMap(InternalBatchesMap& batchesMap);

public:
	~BatchesMap() override;

	void init();
	void render();
	void addRenderer(Ref<Renderer> renderer);

	void forceRegenerateBuffers();
};