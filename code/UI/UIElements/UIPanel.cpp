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

    mTransform->setLocalPosition(mConfig.mDisplayPosition);
    mTransform->setLocalScale(Vector3(mConfig.mDisplaySize, 1));
    mTransform->mAffectedByProjection = false;
    mTransform->mIgnoreParentScale = true;
}

void UIArea::calculateConfig()
{
    mConfig.mDisplayPosition = mConfig.mPosition;
    mConfig.mDisplaySize = mConfig.mSize;

	if (mConfig.mAdjustSizeToText)
	{
        Vector2 textSize(0,0);
        f32 maxAscender = 0;
        f32 maxDescender = 0;
        FOR_ARRAY(i, mConfig.mText)
        {
            char character = mConfig.mText.at(i);
            const FontGlyphData& glyphData = GET_SYSTEM(UIManager).getGlyphData(character);

            textSize.x += glyphData.mAdvance.x;
            maxAscender = std::max(glyphData.mMetrics.mHoriBearing.y, maxAscender);
            maxDescender = std::max(glyphData.mMetrics.mSize.y - glyphData.mMetrics.mHoriBearing.y, maxDescender);
        }
        textSize.y = maxAscender + maxDescender;

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
    rendererData.mMaterialId = mConfig.mMaterialId;
    rendererData.mIsInstanced = true;
    rendererData.mStencilData = calculateStencilData();

    Ptr<MeshRenderer> renderer = createComponent<MeshRenderer>(rendererData);
	renderer->getMaterialInstance().mMaterialInstancedProperties.mDepth = mConfig.mLayer;
	renderer->getMaterialInstance().mMaterialInstancedProperties.mColor = mConfig.mStyle->mBackgroundColor;
    
    setComponentsCache();
}