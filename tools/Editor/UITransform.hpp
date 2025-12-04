#pragma once

#include "UI/Module.hpp"
#include "UIVector.hpp"

class UITransform: public GameObject
{
public:
    virtual void init() override;
    void update(const Transform* transform);
    virtual void onDestroy() override;

private:
    UIVector* mPosition = nullptr;
    UIVector* mRotation = nullptr;
    UIVector* mScale = nullptr;
};
REGISTER_CLASS(UITransform)