#pragma once

#include "UI/Module.hpp"

class UIVector: public SceneObject
{
public:
    virtual void init() override;
    void update(const Vector3& v);
    virtual void onDestroy() override;

private:
    TEntityHandler<UIText> mX;
    TEntityHandler<UIText> mY;
    TEntityHandler<UIText> mZ;
    TEntityHandler<UIText> mW;
};
REGISTER_CLASS(UIVector)