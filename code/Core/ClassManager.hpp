#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Singleton.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Memory.hpp"

#ifdef CPP_INCLUDE
#include "ClassManager.hpp"
#include "Core/Log/Log.hpp"
#endif

#define REGISTER_CLASS_BY_NAME(...) \
    ClassRegister __classRegister ## _ ## __VA_ARGS__ = ClassRegister(#__VA_ARGS__, []() { \
        __VA_ARGS__ *object = nullptr;\
        if constexpr (! std::is_abstract<__VA_ARGS__>::value)\
        {\
            object = NEW(__VA_ARGS__);\
        }\
        else\
        {\
            ASSERT_MSG(false, "Cannot instantiate Abstract Class object " + std::string(#__VA_ARGS__));\
        }\
        return object;\
    });

using ClassRegisterCallback = std::function<ObjectBase*()>;

class ClassRegister
{
private:        
    std::string mClassName;
    ClassRegisterCallback mCallback;
public:
    CPP ClassRegister(const std::string& className, ClassRegisterCallback callback)
    {
        MAP_INSERT(ClassManager::smRegisters, className, callback);
    }

};

class ClassManager: public Singleton<ClassManager>
{
    friend ClassRegister;
    
private:
    inline static std::map<std::string, ClassRegisterCallback> smRegisters;
    std::map<std::string, ClassRegisterCallback> mInstanceByNameMap;

public:
    CPP void init()
    {
        TRACE()
        
        FOR_MAP(it, smRegisters)
        {
            registerClassByName(it->first, it->second);
        }
    }

    CPP void registerClassByName(const std::string& className, ClassRegisterCallback callback)
    {
        MAP_INSERT(mInstanceByNameMap, className, callback);
    }

    template<class T, typename = std::enable_if_t<std::is_base_of<ObjectBase, T>::value> >
    T* instanceByName(const std::string& className)
    {
        return static_cast<T*>(instanceByName(className));
    }

    ObjectBase* instanceByName(const std::string& className)
    {
        if(MAP_CONTAINS(mInstanceByNameMap, className))
        {
            return mInstanceByNameMap.at(className)();
        }

        return nullptr;
    }
};