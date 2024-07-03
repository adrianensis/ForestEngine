#pragma once

#include "Graphics/GPU/GPUMesh.hpp"

class Model;
class Material;

class Mesh: public GPUMesh
{
public:
    Ptr<const Model> mModel;
};
REGISTER_CLASS(Mesh);
