#pragma once

#include "UI/Module.hpp"

class UIVector: public GameObject
{
public:
    virtual void init() override;
    void update(const Maths::Vector3& v);
    virtual void onDestroy() override;

private:
    UIText* mX = nullptr;
    UIText* mY = nullptr;
    UIText* mZ = nullptr;
    UIText* mW = nullptr;
};
REGISTER_CLASS(UIVector)