#pragma once

#include "UI/UIElement.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIPanel.hpp"

#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIStyle.hpp"

#endif

class UIPanel: public UIElement
{
    GENERATE_METADATA(UIPanel)
public:
    CPP void initFromConfig(const UIElementConfig& config) override
    {
        UIElement::initFromConfig(config);

        Vector3 size = mConfig.mSize;
        size.z = 1;
        size.x = size.x / RenderContext::getAspectRatio();

        getTransform().get().setLocalPosition(mConfig.mDisplayPosition);
        getTransform().get().setScale(Vector3(UIUtils::correctAspectRatio_X(mConfig.mSize)));
        getTransform().get().setAffectedByProjection(false);

        Renderer *renderer = NEW(Renderer);
        renderer->init();

        renderer->setMesh(MeshPrimitives::getInstance().getPrimitive<Rectangle>());
        renderer->setMaterial(mConfig.mMaterial);
        renderer->setColor(mConfig.mStyle->mBackgroundColor);
        renderer->setUseDepth(true);
        renderer->setDepth(mConfig.mLayer);
        //renderer->setHasBorder(true);

        addComponent<Renderer>(renderer);
        
        setComponentsCache();
    }
};