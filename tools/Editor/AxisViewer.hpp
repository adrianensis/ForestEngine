#pragma once

#include "UI/Module.hpp"

class SingleAxisViewer: public GameObject
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

REGISTER_CLASS(SingleAxisViewer)

class AxisViewer: public GameObject
{
public:
    void createAxis();
    void update();
    virtual void onDestroy() override;

private:
    Ptr<SingleAxisViewer> mAxisX;
    Ptr<SingleAxisViewer> mAxisY;
    Ptr<SingleAxisViewer> mAxisZ;
};
REGISTER_CLASS(AxisViewer)