#ifndef CLASSMANAGER_HPP
#define CLASSMANAGER_HPP

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Singleton.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Memory.hpp"

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
public:
    ClassRegister(CR(std::string) className, ClassRegisterCallback callback);

private:        
    std::string mClassName;
    ClassRegisterCallback mCallback;
};

class ClassManager: public Singleton<ClassManager>
{
    friend ClassRegister;

public:
    void init();

    void registerClassByName(CR(std::string) className, ClassRegisterCallback callback);

    template<class T, typename = std::enable_if_t<std::is_base_of<ObjectBase, T>::value> >
    T* instanceByName(CR(std::string) className)
    {
        return static_cast<T*>(instanceByName(className));
    }

    ObjectBase* instanceByName(CR(std::string) className)
    {
        if(MAP_CONTAINS(mInstanceByNameMap, className))
        {
            return mInstanceByNameMap.at(className)();
        }

        return nullptr;
    }
        
private:
    inline static std::map<std::string, ClassRegisterCallback> smRegisters;
    std::map<std::string, ClassRegisterCallback> mInstanceByNameMap;
};

#endif