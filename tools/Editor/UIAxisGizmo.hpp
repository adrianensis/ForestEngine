#pragma once

#include "UI/Module.hpp"

class UISingleAxisGizmo: public GameObject
{
public:
    void setAxis(const Line& line, const Vector4& color, HashedString axisName);
    void renderAxisName();
    void update();
    virtual void onDestroy() override;

private:
    Line mAxis;
    Vector4 mColor;
    HashedString mAxisName;
    HashedString mNegAxisName;
    Ptr<UIText> mPositive;
    Ptr<UIText> mNegative;
};

REGISTER_CLASS(UISingleAxisGizmo)

class UIAxisGizmo: public GameObject
{
public:
    void createAxis();
    void update();
    virtual void onDestroy() override;

private:
    Ptr<UISingleAxisGizmo> mAxisX;
    Ptr<UISingleAxisGizmo> mAxisY;
    Ptr<UISingleAxisGizmo> mAxisZ;
};
REGISTER_CLASS(UIAxisGizmo)