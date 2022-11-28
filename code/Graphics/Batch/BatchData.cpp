#include "Graphics/Batch/BatchData.hpp"
#include "Graphics/Renderer/Renderer.hpp"

void BatchData::init(Ptr<Renderer> renderer)
{
    mMaterial = renderer.get().getMaterial();
    mMesh = renderer.get().getMesh();
    mIsStatic = renderer.get().isStatic();
    mIsWorldSpace = renderer.get().getIsWorldSpace();
    mIsInstanced = renderer.get().getIsInstanced();
    mStencilValue = renderer.get().getStencilValue();
    mIsStencilMask = renderer.get().getIsStencilMask();
    mStencilFunction = renderer.get().getStencilFunction();
}
