#include "Core/System/SystemManager.hpp"
#include "Core/Log/Log.hpp"

NS_BEGIN(Core)
void SystemsManager::init()
{

}

void SystemsManager::terminate()
{
    for (auto it = mSystemsInOrder.rbegin(); it != mSystemsInOrder.rend(); ++it)
    {
        (*it)->terminate();
        LOG_TAG("SYSTEM", "Terminating system: " + std::to_string(ClassManager::getDynamicClassMetadata((*it).getInternalPointer()).mClassDefinition.getId()) + " " + ClassManager::getDynamicClassMetadata((*it).getInternalPointer()).mClassDefinition.mName.get())
        mSystems.erase(ClassManager::getDynamicClassMetadata((*it).getInternalPointer()).mClassDefinition.getId());
    }

    mSystemsInOrder.clear();
    mSystems.clear();
}
NS_END