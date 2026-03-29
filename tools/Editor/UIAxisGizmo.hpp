#pragma once

#include "UI/Module.hpp"
#include "UI/UIManager.hpp"

class UISingleAxisGizmo: public GameObject
{
public:
    void setAxis(UIManager* uiManager, const Maths::Line& line, const Maths::Vector4& color, Core::HashedString axisName);
    void renderAxisName();
    void update();
    virtual void onDestroy() override;

private:
    Maths::Line mAxis;
    Maths::Vector4 mColor;
    Core::HashedString mAxisName;
    Core::HashedString mNegAxisName;
    UIText* mPositive = nullptr;
    UIText* mNegative = nullptr;
    UIManager* mUIManager = nullptr;
};

REGISTER_CLASS(UISingleAxisGizmo)

class UIAxisGizmo: public GameObject
{
public:
    void createAxis(UIManager* uiManager);
    void update();
    virtual void onDestroy() override;

private:
    UISingleAxisGizmo* mAxisX = nullptr;
    UISingleAxisGizmo* mAxisY = nullptr;
    UISingleAxisGizmo* mAxisZ = nullptr;
};
REGISTER_CLASS(UIAxisGizmo)