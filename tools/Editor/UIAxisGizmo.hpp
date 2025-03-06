#pragma once

#include "UI/Module.hpp"

class UISingleAxisGizmo: public SceneObject
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
    TEntityHandler<UIText> mPositive;
    TEntityHandler<UIText> mNegative;
};

REGISTER_CLASS(UISingleAxisGizmo)

class UIAxisGizmo: public SceneObject
{
public:
    void createAxis();
    void update();
    virtual void onDestroy() override;

private:
    TEntityHandler<UISingleAxisGizmo> mAxisX;
    TEntityHandler<UISingleAxisGizmo> mAxisY;
    TEntityHandler<UISingleAxisGizmo> mAxisZ;
};
REGISTER_CLASS(UIAxisGizmo)