#pragma once

#include "UI/Module.hpp"

class UISceneTree: public GameObject
{
public:
    virtual void init() override;
    void update();
    virtual void onDestroy() override;

private:
    std::unordered_map<ObjectId, TypedEntityHandler<UIButton>> mTexts;
};
REGISTER_CLASS(UISceneTree)