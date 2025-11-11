#pragma once

#include "UI/Module.hpp"

class UIVector: public GameObject
{
public:
    virtual void init() override;
    void update(const Maths::Vector3& v);
    virtual void onDestroy() override;

private:
    EC::EntityPtr<UIText> mX;
    EC::EntityPtr<UIText> mY;
    EC::EntityPtr<UIText> mZ;
    EC::EntityPtr<UIText> mW;
};
REGISTER_CLASS(UIVector)