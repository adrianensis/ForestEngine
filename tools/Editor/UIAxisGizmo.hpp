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
    TypedEntityHandler<UIText> mPositive;
    TypedEntityHandler<UIText> mNegative;
};

REGISTER_CLASS(UISingleAxisGizmo)

class UIAxisGizmo: public GameObject
{
public:
    void createAxis();
    void update();
    virtual void onDestroy() override;

private:
    TypedEntityHandler<UISingleAxisGizmo> mAxisX;
    TypedEntityHandler<UISingleAxisGizmo> mAxisY;
    TypedEntityHandler<UISingleAxisGizmo> mAxisZ;
};
REGISTER_CLASS(UIAxisGizmo)