#pragma once

#include "UI/Module.hpp"

class UISingleAxisGizmo: public GameObject
{
public:
    void setAxis(const Maths::Line& line, const Maths::Vector4& color, Core::HashedString axisName);
    void renderAxisName();
    void update();
    virtual void onDestroy() override;

private:
    Maths::Line mAxis;
    Maths::Vector4 mColor;
    Core::HashedString mAxisName;
    Core::HashedString mNegAxisName;
    EC::EntityPtr<UIText> mPositive;
    EC::EntityPtr<UIText> mNegative;
};

REGISTER_CLASS(UISingleAxisGizmo)

class UIAxisGizmo: public GameObject
{
public:
    void createAxis();
    void update();
    virtual void onDestroy() override;

private:
    EC::EntityPtr<UISingleAxisGizmo> mAxisX;
    EC::EntityPtr<UISingleAxisGizmo> mAxisY;
    EC::EntityPtr<UISingleAxisGizmo> mAxisZ;
};
REGISTER_CLASS(UIAxisGizmo)