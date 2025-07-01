#pragma once

#include "UI/Module.hpp"
#include "UIVector.hpp"

class UITransform: public SceneObject
{
public:
    virtual void init() override;
    void update(EC::TComponentPtr<const Transform> transform);
    virtual void onDestroy() override;

private:
    EC::TEntityPtr<UIVector> mPosition;
    EC::TEntityPtr<UIVector> mRotation;
    EC::TEntityPtr<UIVector> mScale;
};
REGISTER_CLASS(UITransform)