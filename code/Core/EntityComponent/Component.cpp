#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Core/EntityComponent/EntityComponentPool.hpp"

NS_BEGIN(EC)
Component::Component()
{
}

Component::~Component()
{

}

void Component::onECComponentRecycle(Core::PoolElementPtr poolElementPtr)
{
    mPoolElementPtr = poolElementPtr;
}

NS_END