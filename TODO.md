# TODO

## Renderer: Add multiple meshes support for complex models and animations

- Renderer
    - change mMehs by mMehses
- Mesh
    - start using mMaterialPath as the official material reference, not Renderer::mMaterial
- RenderEngine::addRenderer(Renderer r)
    - for(m in r.mMeshes)
    - mBatchesMap.add(r, BatchKey(m, texture, shader...));