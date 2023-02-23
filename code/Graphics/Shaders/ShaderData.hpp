#pragma once

#include "Core/Module.hpp"

// TODO: this data is supposed to be fed to the ShaderBuilder, somehow
// it will require a big refactor of MeshBuffer and MeshBatcher, probably Batch as well...



class BufferData
{
public:

};

class ShaderData: public ObjectBase
{
    GENERATE_METADATA(ShaderData)
public:
    std::vector<BufferData> mBuffers;
};
