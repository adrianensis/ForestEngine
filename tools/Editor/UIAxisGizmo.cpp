#include "UIAxisGizmo.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"

#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"

void UISingleAxisGizmo::setAxis(const Maths::Line& line, const Maths::Vector4& color, Core::HashedString axisName)
{
    mAxis = line;
    mColor = color;
    mAxisName = axisName;
    mNegAxisName = Core::HashedString("-" + mAxisName.get());

    EC::TEntityPtr<SceneObject> cameraSceneObject = GET_SYSTEM(ScenesManager).getCameraSceneObject();
    EC::TComponentPtr<Camera> camera = ECManager.getFirstComponent<Camera>(cameraSceneObject);

    Maths::Vector3 startLine = mTransform->getModelMatrixNoScale().mulVector(
            Maths::Vector4(UIUtils::correctAspectRatioVectorX(
                cameraSceneObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getStart(), 1))
                ), 1)
        );
    Maths::Vector3 endLine = mTransform->getModelMatrixNoScale().mulVector(
            Maths::Vector4(UIUtils::correctAspectRatioVectorX(
                cameraSceneObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getEnd(), 1))
                ), 1)
        );
    GET_SYSTEM(DebugRenderer).drawLine(Maths::Line(startLine, endLine), 1, Maths::GeometricSpace::SCREEN, mColor);

    Maths::Vector3 startGlyph = Maths::Vector4(
        UIUtils::correctAspectRatioVectorX(
                cameraSceneObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getStart(), 1))
            )
        , 1);
    Maths::Vector3 endGlyph = Maths::Vector4(
        UIUtils::correctAspectRatioVectorX(
                cameraSceneObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getEnd(), 1))
            )
        , 1);
    
    UIBuilder uiBuilder;
	uiBuilder.
	// setPosition(Maths::Vector2(0,0)).
	setPosition(startGlyph).
    setLayer(startGlyph.z).
	setAdjustSizeToText(true).
    setIsStaticText(false).
	setIsAffectedByLayout(false).
    setParent(this).
	setTextScale(0.5f);

    mPositive = uiBuilder.
	setText(mAxisName).
	create<UIText>().
	getUIElement<UIText>();

    uiBuilder.restoreAll();

    uiBuilder.
	// setPosition(Maths::Vector2(0,0)).
	setPosition(endGlyph).
    setLayer(endGlyph.z).
	setAdjustSizeToText(true).
    setIsStaticText(false).
	setIsAffectedByLayout(false).
    setParent(this).
	setTextScale(0.5f);

    mNegative = uiBuilder.
	setText(mNegAxisName).
	create<UIText>().
	getUIElement<UIText>();
}

void UISingleAxisGizmo::update()
{
    EC::TEntityPtr<SceneObject> cameraSceneObject = GET_SYSTEM(ScenesManager).getCameraSceneObject();
    EC::TComponentPtr<Camera> camera = ECManager.getFirstComponent<Camera>(cameraSceneObject);

    Maths::Vector3 startLine = mTransform->getModelMatrixNoScale().mulVector(
            Maths::Vector4(UIUtils::correctAspectRatioVectorX(
                cameraSceneObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getStart(), 1))
                ), 1)
        );
    Maths::Vector3 endLine = mTransform->getModelMatrixNoScale().mulVector(
            Maths::Vector4(UIUtils::correctAspectRatioVectorX(
                cameraSceneObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getEnd(), 1))
                ), 1)
        );
    GET_SYSTEM(DebugRenderer).drawLine(Maths::Line(startLine, endLine), 1, Maths::GeometricSpace::SCREEN, mColor);

    // GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mPositive);
    // GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mNegative);

    Maths::Vector3 startGlyph = Maths::Vector4(
        UIUtils::correctAspectRatioVectorX(
                cameraSceneObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getStart(), 1))
            )
        , 1);
    Maths::Vector3 endGlyph = Maths::Vector4(
        UIUtils::correctAspectRatioVectorX(
                cameraSceneObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getEnd(), 1))
            )
        , 1);

    mPositive->mTransform->setLocalPosition(startGlyph);
    mNegative->mTransform->setLocalPosition(endGlyph);
}

void UISingleAxisGizmo::onDestroy()
{
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mPositive);
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mNegative);
    SceneObject::onDestroy();
}

void UIAxisGizmo::createAxis()
{
    Maths::Vector3 axisHalfSize(0.065, 0.065, 0.065);
    Maths::Line axisX = Maths::Line(Maths::Vector3(1,0,0)*axisHalfSize, Maths::Vector3(-1,0,0)*axisHalfSize);
    Maths::Line axisY = Maths::Line(Maths::Vector3(0,1,0)*axisHalfSize, Maths::Vector3(0,-1,0)*axisHalfSize);
    Maths::Line axisZ = Maths::Line(Maths::Vector3(0,0,1)*axisHalfSize, Maths::Vector3(0,0,-1)*axisHalfSize);

    mAxisX = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UISingleAxisGizmo>();
    mAxisY = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UISingleAxisGizmo>();
    mAxisZ = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UISingleAxisGizmo>();

    mTransform->addChild(mAxisX->mTransform);
    mTransform->addChild(mAxisY->mTransform);
    mTransform->addChild(mAxisZ->mTransform);

    mAxisX->setAxis(axisX, Maths::Vector4(1,0,0,1), "X");
    mAxisY->setAxis(axisY, Maths::Vector4(0,1,0,1), "Y");
    mAxisZ->setAxis(axisZ, Maths::Vector4(0,0,1,1), "Z");
}

void UIAxisGizmo::update()
{
    mAxisX->update();
    mAxisY->update();
    mAxisZ->update();
}

void UIAxisGizmo::onDestroy()
{
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mAxisX);
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mAxisY);
    GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mAxisZ);
    SceneObject::onDestroy();
}