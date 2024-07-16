#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/GPU/GPUProgram.hpp"

class GPUVertexBuffersContainer;
class GPUSharedBuffersContainer;
class Material;

class ShaderManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    template<class T> T_EXTENDS(T, Shader)
    Ptr<T> createShader()
    {   
        ClassId classId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        if(!mShaders.contains(classId))
        {
            mShaders.emplace(classId, OwnerPtr<Shader>::moveCast(OwnerPtr<T>::newObject()));
            mShaders.at(classId)->init();
        }

        return getShader<T>();
    }

    template<class T> T_EXTENDS(T, Shader)
    Ptr<T> getShader() const
    {
        ClassId classId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        if(mShaders.contains(classId))
        {
            return Ptr<T>::cast(mShaders.at(classId));
        }

        return Ptr<T>();
    }

    OwnerPtr<GPUProgram> compileShader(HashedString label, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const Material& material, Ptr<const Shader> customShader);

private:
    std::unordered_map<ClassId, OwnerPtr<Shader>> mShaders;
public:
    CRGET(Shaders)
};
REGISTER_CLASS(ShaderManager);
