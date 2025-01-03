#include "Core/System/SystemManager.hpp"

void SystemsManager::init()
{

}

void SystemsManager::terminate()
{
    for (auto it = mSystemsInOrder.rbegin(); it != mSystemsInOrder.rend(); ++it)
    {
        (*it)->terminate();
        mSystems.erase(ClassManager::getDynamicClassMetadata((*it).getInternalPointer()).mClassDefinition.getId());
    }

    mSystemsInOrder.clear();
}