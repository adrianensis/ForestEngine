#pragma once

#include "Engine/Core.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/Event/Event.hpp"

class Transform;
class Scene;

class GameComponent: public EC::Component, public Event::IEventObject
{
public:
    GameComponent();

    bool isActive() const;
    void setIsActive(bool isActive);
    EC::Entity* getOwnerEntity() const;
    virtual void onDestroy();
    
private:
    virtual void onECComponentDestroyed();
	
    bool mIsActive = true;
	bool mIsDestroyed = false;

	Core::u64 mComponentId = 0;
    // Important: starts by 1, 0 is reserved for null
	inline static Core::u64 smComponentIdCounter = 1;

public:
	GET(IsDestroyed)
    GET(ComponentId)
};
REGISTER_CLASS(GameComponent);