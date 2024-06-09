#include "UIAxisGizmo.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"

void UISingleAxisGizmo::setAxis(const Line& line, const Vector4& color, HashedString axisName)
{
    mAxis = line;
    mColor = color;
    mAxisName = axisName;
    mNegAxisName = HashedString("-" + mAxisName.get());

    Ptr<GameObject> cameraGameObject = mScene->getCameraGameObject();
    Ptr<Camera> camera = cameraGameObject->getFirstComponent<Camera>();

    Vector3 startLine = mTransform->getModelMatrixNoScale().mulVector(
            Vector4(UIUtils::correctAspectRatio_X(
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Vector4(mAxis.getStart(), 1))
                ), 1)
        );
    Vector3 endLine = mTransform->getModelMatrixNoScale().mulVector(
            Vector4(UIUtils::correctAspectRatio_X(
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Vector4(mAxis.getEnd(), 1))
                ), 1)
        );
    GET_SYSTEM(DebugRenderer).drawLine(Line(startLine, endLine), 1, GeometricSpace::SCREEN, mColor);

    Vector3 startGlyph = Vector4(
        UIUtils::correctAspectRatio_X(
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Vector4(mAxis.getStart(), 1))
            )
        , 1);
    Vector3 endGlyph = Vector4(
        UIUtils::correctAspectRatio_X(
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Vector4(mAxis.getEnd(), 1))
            )
        , 1);
    
    UIBuilder uiBuilder;
	uiBuilder.
	// setPosition(Vector2(0,0)).
	setPosition(startGlyph).
    setLayer(startGlyph.z).
	setAdjustSizeToText(true).
	setIsAffectedByLayout(false).
    setParent(getPtrToThis<GameObject>()).
	setSize(Vector2(0.5/2.0, 0.05f/2.0));

    mPositive = uiBuilder.
	setText(mAxisName).
    setIsStatic(false).
	create<UIText>().
	getUIElement<UIText>();

    uiBuilder.restoreAll();

    uiBuilder.
	// setPosition(Vector2(0,0)).
	setPosition(endGlyph).
    setLayer(endGlyph.z).
	setAdjustSizeToText(true).
	setIsAffectedByLayout(false).
    setParent(getPtrToThis<GameObject>()).
	setSize(Vector2(0.5/2.0, 0.05f/2.0));

    mNegative = uiBuilder.
	setText(mNegAxisName).
    setIsStatic(false).
	create<UIText>().
	getUIElement<UIText>();
}

void UISingleAxisGizmo::update()
{
    Ptr<GameObject> cameraGameObject = mScene->getCameraGameObject();
    Ptr<Camera> camera = cameraGameObject->getFirstComponent<Camera>();

    Vector3 startLine = mTransform->getModelMatrixNoScale().mulVector(
            Vector4(UIUtils::correctAspectRatio_X(
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Vector4(mAxis.getStart(), 1))
                ), 1)
        );
    Vector3 endLine = mTransform->getModelMatrixNoScale().mulVector(
            Vector4(UIUtils::correctAspectRatio_X(
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Vector4(mAxis.getEnd(), 1))
                ), 1)
        );
    GET_SYSTEM(DebugRenderer).drawLine(Line(startLine, endLine), 1, GeometricSpace::SCREEN, mColor);

    // GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mPositive);
    // GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mNegative);

    Vector3 startGlyph = Vector4(
        UIUtils::correctAspectRatio_X(
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Vector4(mAxis.getStart(), 1))
            )
        , 1);
    Vector3 endGlyph = Vector4(
        UIUtils::correctAspectRatio_X(
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Vector4(mAxis.getEnd(), 1))
            )
        , 1);

    mPositive->mTransform->setLocalPosition(startGlyph);
    mNegative->mTransform->setLocalPosition(endGlyph);
}

void UISingleAxisGizmo::onDestroy()
{
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mPositive);
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mNegative);
    GameObject::onDestroy();
}

void UIAxisGizmo::createAxis()
{
    Vector3 axisHalfSize(0.1, 0.1, 0.1);
    Line axisX = Line(Vector3(1,0,0)*axisHalfSize, Vector3(-1,0,0)*axisHalfSize);
    Line axisY = Line(Vector3(0,1,0)*axisHalfSize, Vector3(0,-1,0)*axisHalfSize);
    Line axisZ = Line(Vector3(0,0,1)*axisHalfSize, Vector3(0,0,-1)*axisHalfSize);

    mAxisX = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<UISingleAxisGizmo>();
    mAxisY = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<UISingleAxisGizmo>();
    mAxisZ = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<UISingleAxisGizmo>();

    mTransform->addChild(mAxisX->mTransform);
    mTransform->addChild(mAxisY->mTransform);
    mTransform->addChild(mAxisZ->mTransform);

    mAxisX->setAxis(axisX, Vector4(1,0,0,1), "X");
    mAxisY->setAxis(axisY, Vector4(0,1,0,1), "Y");
    mAxisZ->setAxis(axisZ, Vector4(0,0,1,1), "Z");
}

void UIAxisGizmo::update()
{
    mAxisX->update();
    mAxisY->update();
    mAxisZ->update();
}

void UIAxisGizmo::onDestroy()
{
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mAxisX);
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mAxisY);
    GET_SYSTEM(ScenesManager).getCurrentScene()->removeGameObject(mAxisZ);
    GameObject::onDestroy();
}