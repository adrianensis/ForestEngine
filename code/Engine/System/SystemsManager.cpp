#include "Engine/System/SystemsManager.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Metadata/ClassManager.hpp"

NS_BEGIN(System)
void SystemsManager::init()
{

}

void SystemsManager::terminate()
{
    for (auto it = mSystemsInOrder.rbegin(); it != mSystemsInOrder.rend(); ++it)
    {
        const Core::ClassDefinition& classDef = Core::ClassManager::getDynamicClassMetadata(
            (*it).getInternalPointer()).mClassDefinition;
            
        (*it)->terminate();
        LOG_TAG("SYSTEM", "Terminating system: " + 
            std::to_string(classDef.getId()) + " " +
            classDef.mName.get())
        mSystems.erase(classDef.getId());
    }

    mSystemsInOrder.clear();
    mSystems.clear();
}
NS_END