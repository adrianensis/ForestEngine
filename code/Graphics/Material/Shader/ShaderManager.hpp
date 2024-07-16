#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Graphics/Material/Shader/Shader.hpp"

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

private:
    std::unordered_map<ClassId, OwnerPtr<Shader>> mShaders;
public:
    CRGET(Shaders)
};
REGISTER_CLASS(ShaderManager);
