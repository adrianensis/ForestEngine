#pragma once

#include "UI/Module.hpp"
#include "UIVector.hpp"

class UITransform: public SceneObject
{
public:
    virtual void init() override;
    void update(TComponentHandler<const Transform> transform);
    virtual void onDestroy() override;

private:
    TEntityHandler<UIVector> mPosition;
    TEntityHandler<UIVector> mRotation;
    TEntityHandler<UIVector> mScale;
};
REGISTER_CLASS(UITransform)