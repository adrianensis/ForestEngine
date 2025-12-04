#include "Core/EntityComponent/Entity.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/EntityComponentPool.hpp"

NS_BEGIN(EC)
Entity::Entity()
{

}

void Entity::onECComponentRecycle(Core::PoolElementPtr poolElementPtr)
{
    mPoolElementPtr = poolElementPtr;
};
NS_END