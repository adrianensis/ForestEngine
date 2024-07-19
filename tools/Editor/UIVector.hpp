#pragma once

#include "UI/Module.hpp"

class UIVector: public GameObject
{
public:
    virtual void init() override;
    void update(const Vector3& v);
    virtual void onDestroy() override;

private:
    TypedEntityHandler<UIText> mX;
    TypedEntityHandler<UIText> mY;
    TypedEntityHandler<UIText> mZ;
    TypedEntityHandler<UIText> mW;
};
REGISTER_CLASS(UIVector)