#pragma once

#include "UI/Module.hpp"

class UISingleAxisGizmo: public SceneObject
{
public:
    void setAxis(const Line& line, const Vector4& color, Core::HashedString axisName);
    void renderAxisName();
    void update();
    virtual void onDestroy() override;

private:
    Line mAxis;
    Vector4 mColor;
    Core::HashedString mAxisName;
    Core::HashedString mNegAxisName;
    EC::TEntityPtr<UIText> mPositive;
    EC::TEntityPtr<UIText> mNegative;
};

REGISTER_CLASS(UISingleAxisGizmo)

class UIAxisGizmo: public SceneObject
{
public:
    void createAxis();
    void update();
    virtual void onDestroy() override;

private:
    EC::TEntityPtr<UISingleAxisGizmo> mAxisX;
    EC::TEntityPtr<UISingleAxisGizmo> mAxisY;
    EC::TEntityPtr<UISingleAxisGizmo> mAxisZ;
};
REGISTER_CLASS(UIAxisGizmo)