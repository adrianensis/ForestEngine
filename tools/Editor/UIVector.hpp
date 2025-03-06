#pragma once

#include "UI/Module.hpp"

class UIVector: public SceneObject
{
public:
    virtual void init() override;
    void update(const Vector3& v);
    virtual void onDestroy() override;

private:
    TEntityPtr<UIText> mX;
    TEntityPtr<UIText> mY;
    TEntityPtr<UIText> mZ;
    TEntityPtr<UIText> mW;
};
REGISTER_CLASS(UIVector)