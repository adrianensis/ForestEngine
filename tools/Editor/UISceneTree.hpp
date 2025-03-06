#pragma once

#include "UI/Module.hpp"

class UISceneTree: public SceneObject
{
public:
    virtual void init() override;
    void update();
    virtual void onDestroy() override;

private:
    // std::unordered_map<, TEntityHandler<UIButton>> mTexts;
};
REGISTER_CLASS(UISceneTree)