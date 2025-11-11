#pragma once

#include "Engine/Core.hpp"
#include "Engine/EntityComponent/Entity.hpp"
#include "Engine/EntityComponent/Component.hpp"

class Transform;
class Scene;

class GameComponent: public EC::Component, public Event::IEventObject
{
public:
    GameComponent();
    
    EC::EntityPtrBase getOwnerEntity() const;

private:
	Core::u64 mComponentId = 0;
    // Important: starts by 1, 0 is reserved for null
	inline static Core::u64 smComponentIdCounter = 1;

public:
    GET(ComponentId)
};
REGISTER_CLASS(GameComponent);