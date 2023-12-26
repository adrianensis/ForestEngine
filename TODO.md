# TODO

- [ ] OpenGL - Cache uniform locations
- [ ] Hide backface for 2D sprites (optional, since some sprites could rotate?)
- [ ] Test frustum culling, enable back
- [ ] Add Option for SSBO to be readonly

## Interesting / Nice To Have / Optimizations
- [ ] Animation Blending
- [ ] OpenGL - Compress textures? Why/When?
- [ ] OpenGL - Bindless Textures
- [ ] OpenGL - DSA - Direct State Access
- Use Octree to calculate visibility? (frustum culling with octree?). Don't put model matrix in UBO if occluded.
- Threads - split render and logic in different threads
  - Scripting Engine - Put scripting engine in another thread???
  - Tasks system - threaded?
- Implement "next frame" timers

### ECS (cache friendly)
- Components live in Pools/Comp.Managers
- Inheritance is a 1st class citizen in OOP, composition is a 1st class citizen in ECS.
- OOP encourages encapsulation of data, ECS encourages exposed POD (plain old data) objects.
- OOP colocates data with behavior, ECS separates data from behavior.
- OOP Object instances are of a single static type, ECS entities can have multiple, dynamically changing components

- [from here](https://gamedev.stackexchange.com/questions/82030/how-are-entity-systems-cache-efficient)
  - No inheritance/virtual functions allowed since jumping to vtable mem address generate cache misses
  - That should go for systems
  - QuadTree could live in a packed array, better cache performance

## Bugs
- -

## Potential Bugs
- RenderContext::createVBO() performs a glBindBuffer but createEBO() and createUBO() don't. Is that correct? Unify in a single createBuffer() method?