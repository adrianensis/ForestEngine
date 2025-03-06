#pragma once

#include "UI/Module.hpp"
#include "UIVector.hpp"

class UITransform: public SceneObject
{
public:
    virtual void init() override;
    void update(TComponentPtr<const Transform> transform);
    virtual void onDestroy() override;

private:
    TEntityPtr<UIVector> mPosition;
    TEntityPtr<UIVector> mRotation;
    TEntityPtr<UIVector> mScale;
};
REGISTER_CLASS(UITransform)