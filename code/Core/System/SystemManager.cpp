#include "Core/System/SystemManager.hpp"
#include "Core/Log/Log.hpp"

void SystemsManager::init()
{

}

void SystemsManager::terminate()
{
    for (auto it = mSystemsInOrder.rbegin(); it != mSystemsInOrder.rend(); ++it)
    {
        (*it)->terminate();
        LOG_TAG("SYSTEM", "Terminating system: " + std::to_string(Core::ClassManager::getDynamicClassMetadata((*it).getInternalPointer()).mClassDefinition.getId()) + " " + Core::ClassManager::getDynamicClassMetadata((*it).getInternalPointer()).mClassDefinition.mName.get())
        mSystems.erase(Core::ClassManager::getDynamicClassMetadata((*it).getInternalPointer()).mClassDefinition.getId());
    }

    mSystemsInOrder.clear();
    mSystems.clear();
}