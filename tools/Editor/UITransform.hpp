#pragma once

#include "UI/Module.hpp"
#include "UIVector.hpp"

class UITransform: public GameObject
{
public:
    virtual void init() override;
    void update(EC::ComponentPtr<const Transform> transform);
    virtual void onDestroy() override;

private:
    EC::EntityPtr<UIVector> mPosition;
    EC::EntityPtr<UIVector> mRotation;
    EC::EntityPtr<UIVector> mScale;
};
REGISTER_CLASS(UITransform)