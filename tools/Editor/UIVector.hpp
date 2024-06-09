#pragma once

#include "UI/Module.hpp"

class UIVector: public GameObject
{
public:
    virtual void init() override;
    void update(const Vector3& v);
    virtual void onDestroy() override;

private:
    Ptr<UIText> mX;
    Ptr<UIText> mY;
    Ptr<UIText> mZ;
    Ptr<UIText> mW;
};
REGISTER_CLASS(UIVector)