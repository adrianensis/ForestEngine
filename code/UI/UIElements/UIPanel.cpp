#include "UI/UIElements/UIPanel.hpp"

#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"

void UIArea::initFromConfig(const UIElementConfig& config) 
{
    UIElement::initFromConfig(config);

    if(mConfig.mParent)
    {
        mConfig.mParent->mTransform->addChild(mTransform);
    }

    mTransform->setPosition(mConfig.mDisplayPosition);
    mTransform->setScale(Vector3(mConfig.mDisplaySize, 1));
    mTransform->mAffectedByProjection = false;
    mTransform->mIgnoreParentScale = true;
}

void UIArea::calculateConfig()
{
    mConfig.mDisplayPosition = mConfig.mPosition;
    mConfig.mDisplaySize = mConfig.mSize;

	if (mConfig.mAdjustSizeToText)
	{
        Vector2 textSize;
        FOR_ARRAY(i, mConfig.mText)
        {
            char character = mConfig.mText.at(i);
            Vector2 glyphSize = GET_SYSTEM(UIManager).getGlyphData(character).mMetrics.mSize;
            textSize.x += glyphSize.x;
            textSize.y = std::max(glyphSize.y, textSize.y);
        }

		mConfig.mSize = textSize;
		mConfig.mDisplaySize = UIUtils::toScreenSpace(textSize);
	}

    // translate to top left corner
	mConfig.mDisplayPosition.x += mConfig.mDisplaySize.x / 2.0f;
    mConfig.mDisplayPosition.y -= mConfig.mDisplaySize.y / 2.0f;
}

void UIPanel::initFromConfig(const UIElementConfig& config) 
{
    UIArea::initFromConfig(config);

    RendererData rendererData;
    rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
    rendererData.mMaterial = (mConfig.mMaterial);
    rendererData.mStencilData = calculateStencilData();

    Ptr<MeshRenderer> renderer = createComponent<MeshRenderer>(rendererData);
    renderer->mUseDepth = (true);
	renderer->setDepth(mConfig.mLayer);
	renderer->setColor(mConfig.mStyle->mBackgroundColor);
    
    setComponentsCache();
}