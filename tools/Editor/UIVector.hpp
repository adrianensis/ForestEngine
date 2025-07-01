#pragma once

#include "UI/Module.hpp"

class UIVector: public SceneObject
{
public:
    virtual void init() override;
    void update(const Vector3& v);
    virtual void onDestroy() override;

private:
    EC::TEntityPtr<UIText> mX;
    EC::TEntityPtr<UIText> mY;
    EC::TEntityPtr<UIText> mZ;
    EC::TEntityPtr<UIText> mW;
};
REGISTER_CLASS(UIVector)