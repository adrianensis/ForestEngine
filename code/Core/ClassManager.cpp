#include "ClassManager.hpp"
#include "Core/Log/Log.hpp"


ClassRegister::ClassRegister(const String& className, ClassRegisterCallback callback)
{
    MAP_INSERT(ClassManager::smRegisters, className, callback);
}

void ClassManager::init()
{
    TRACE()
    
    FOR_MAP(it, smRegisters)
    {
        registerClassByName(it->first, it->second);
    }
}

void ClassManager::registerClassByName(const String& className, ClassRegisterCallback callback)
{
    MAP_INSERT(mInstanceByNameMap, className, callback);
}
