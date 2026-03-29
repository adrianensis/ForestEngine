#include "Core/System/SystemsManager.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Memory/Memory.hpp"
#include "Core/Metadata/ClassManager.hpp"

NS_BEGIN(System)
void SystemsManager::init()
{
    LOG_TAG("SYSTEM", "Init SystemsManager");
}

void SystemsManager::terminate()
{
    LOG_TAG("SYSTEM", "Terminate SystemsManager");

    for (auto it = mSystemsInOrder.rbegin(); it != mSystemsInOrder.rend(); ++it)
    {
        const Core::ClassDefinition& classDef = Core::ClassManager::getDynamicClassMetadata(*it).mClassDefinition;
            
        (*it)->terminate();
        Core::Memory::deleteObject(*it);
        LOG_TAG("SYSTEM", "Terminating system: " + 
            std::to_string(classDef.getId()) + " " +
            classDef.mName.get())
        mSystems.erase(classDef.getId());
    }

    mSystemsInOrder.clear();
    mSystems.clear();
}
NS_END