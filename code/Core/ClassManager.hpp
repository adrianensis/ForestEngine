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
            ASSERT_MSG(false, "Cannot instantiate Abstract Class object " + String(#__VA_ARGS__));\
        }\
        return object;\
    });

using ClassRegisterCallback = std::function<ObjectBase*()>;

class ClassRegister
{
public:
    ClassRegister(const String& className, ClassRegisterCallback callback);

private:        
    String mClassName;
    ClassRegisterCallback mCallback;
};

class ClassManager: public Singleton<ClassManager>
{
    friend ClassRegister;

public:
    void init();

    void registerClassByName(const String& className, ClassRegisterCallback callback);

    template<class T, typename = std::enable_if_t<std::is_base_of<ObjectBase, T>::value> >
    T* instanceByName(const String& className)
    {
        return static_cast<T*>(instanceByName(className));
    }

    ObjectBase* instanceByName(const String& className)
    {
        if(MAP_CONTAINS(mInstanceByNameMap, className))
        {
            return mInstanceByNameMap.at(className)();
        }

        return nullptr;
    }
        
private:
    inline static std::map<String, ClassRegisterCallback> smRegisters;
    std::map<String, ClassRegisterCallback> mInstanceByNameMap;
};

#endif