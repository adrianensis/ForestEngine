#pragma once

#include "Engine/Core.hpp"
#include "Engine/EntityComponent/Entity.hpp"
#include "Engine/EntityComponent/Component.hpp"

class Transform;
class Scene;

class GameComponent: public EC::Component, public Event::IEventObject
{
public:
    EC::EntityPtrBase getOwnerEntity() const;
};
REGISTER_CLASS(GameComponent);