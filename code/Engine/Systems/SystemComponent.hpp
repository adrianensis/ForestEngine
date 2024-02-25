#pragma once

#include "Core/Std.hpp"
#include "Core/Object/ObjectBase.hpp"

class SystemComponent: public ObjectBase
{
public:
    // Important: Override this in ONLY those component classes allowed to be injected into engine systems
    // MeshRenderer for RenderEngine, Script for RenderEngine, ...
    // This will automatically work in derived classes, no need to override this method in derived classes
    virtual ClassId getSystemComponentId() const { return 0; }
public:
    bool mAlreadyAddedToSystem = false;
};
REGISTER_CLASS(SystemComponent);