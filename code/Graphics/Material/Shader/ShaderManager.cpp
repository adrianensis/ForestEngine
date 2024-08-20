#include "Graphics/Material/Shader/ShaderManager.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Material.hpp"

void ShaderManager::init()
{
}

void ShaderManager::terminate()
{
    mShaders.clear();
}