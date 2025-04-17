#pragma once

#include "Core/EntityComponent/ComponentPtr.hpp"
#include "SpacePartition/OcTree.hpp"

#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShader.hpp"
#include "GPU/RenderItem/GPURenderItem.hpp"
#include "Graphics/Renderer/TextureAnimation/TextureAnimation.hpp"

class TextureAnimation;

class MeshRenderer: public Component, public IOcTreeElement
{
    
	DECLARE_SERIALIZATION()
	
public:
    ClassId getComponentTypeId() const override;

    void init(const GPURenderItemData& data);
    void onComponentAdded() override;
    void onDestroy() override;
    void update();

    const GPURenderItemData& getGPURenderItemData() const
    {
        return mGPURenderItem->getGPURenderItemData();
    }

private:
    void calculateRendererModelMatrix();
    const TextureAnimation* getCurrentTextureAnimation() const;
    void updateTextureRegion();

private:
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    bool mUpdateMatrix = false;

    OwnerPtr<GPURenderItem> mGPURenderItem;

public:
    HashedString mCurrentTextureAnimationKey;

public:
    GET(GPURenderItem)
    GET_SET(UpdateMatrix)
};
REGISTER_CLASS(MeshRenderer);
