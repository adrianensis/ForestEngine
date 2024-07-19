#pragma once

#include "UI/Module.hpp"
#include "UIVector.hpp"

class UITransform: public GameObject
{
public:
    virtual void init() override;
    void update(TypedComponentHandler<const Transform> transform);
    virtual void onDestroy() override;

private:
    TypedEntityHandler<UIVector> mPosition;
    TypedEntityHandler<UIVector> mRotation;
    TypedEntityHandler<UIVector> mScale;
};
REGISTER_CLASS(UITransform)