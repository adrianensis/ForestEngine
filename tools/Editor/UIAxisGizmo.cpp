#include "UIAxisGizmo.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"

#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Input/Input.hpp"

void UISingleAxisGizmo::setAxis(const Maths::Line& line, const Maths::Vector4& color, Core::HashedString axisName)
{
    mAxis = line;
    mColor = color;
    mAxisName = axisName;
    mNegAxisName = Core::HashedString("-" + mAxisName.get());

    GameObject* cameraGameObject = mScene->getScenesManager()->getCameraGameObject();

    Maths::Vector3 startLine = mTransform->getModelMatrixNoScale().mulVector(
            Maths::Vector4(UIUtils::correctAspectRatioVectorX(
                    mSystemsDI.getSystem<UIManager>()->getWindow(), 
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getStart(), 1))
                ), 1)
        );
    Maths::Vector3 endLine = mTransform->getModelMatrixNoScale().mulVector(
            Maths::Vector4(UIUtils::correctAspectRatioVectorX(
                    mSystemsDI.getSystem<UIManager>()->getWindow(), 
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getEnd(), 1))
                ), 1)
        );
    GET_SYSTEM(DebugRenderer).drawLine(Maths::Line(startLine, endLine), 1, Maths::GeometricSpace::SCREEN, mColor);

    Maths::Vector3 startGlyph = Maths::Vector4(
        UIUtils::correctAspectRatioVectorX(
                mSystemsDI.getSystem<UIManager>()->getWindow(), 
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getStart(), 1))
            )
        , 1);
    Maths::Vector3 endGlyph = Maths::Vector4(
        UIUtils::correctAspectRatioVectorX(
                mSystemsDI.getSystem<UIManager>()->getWindow(), 
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getEnd(), 1))
            )
        , 1);
    
    UIBuilder uiBuilder = mSystemsDI.getSystem<UIManager>()->createUIBuilder();
	uiBuilder.
	// setPosition(Maths::Vector2(0,0)).
	setPosition(startGlyph).
    setLayer(startGlyph.z).
	setAdjustSizeToText(true).
    setIsStaticText(false).
	setIsAffectedByLayout(false).
    setParent(this);

    mPositive = uiBuilder.
	setText(mAxisName).
	create<UIText>(mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer()).
	getUIElement<UIText>();

    uiBuilder.restoreAll();

    uiBuilder.
	// setPosition(Maths::Vector2(0,0)).
	setPosition(endGlyph).
    setLayer(endGlyph.z).
	setAdjustSizeToText(true).
    setIsStaticText(false).
	setIsAffectedByLayout(false).
    setParent(this);

    mNegative = uiBuilder.
	setText(mNegAxisName).
	create<UIText>(mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer()).
	getUIElement<UIText>();
}

void UISingleAxisGizmo::update()
{
    GameObject* cameraGameObject = mScene->getScenesManager()->getCameraGameObject();

    Maths::Vector3 startLine = mTransform->getModelMatrixNoScale().mulVector(
            Maths::Vector4(UIUtils::correctAspectRatioVectorX(
                    mSystemsDI.getSystem<UIManager>()->getWindow(), 
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getStart(), 1))
                ), 1)
        );
    Maths::Vector3 endLine = mTransform->getModelMatrixNoScale().mulVector(
            Maths::Vector4(UIUtils::correctAspectRatioVectorX(
                    mSystemsDI.getSystem<UIManager>()->getWindow(), 
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getEnd(), 1))
                ), 1)
        );
    GET_SYSTEM(DebugRenderer).drawLine(Maths::Line(startLine, endLine), 1, Maths::GeometricSpace::SCREEN, mColor);

    Maths::Vector3 startGlyph = Maths::Vector4(
        UIUtils::correctAspectRatioVectorX(
                mSystemsDI.getSystem<UIManager>()->getWindow(), 
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getStart(), 1))
            )
        , 1);
    Maths::Vector3 endGlyph = Maths::Vector4(
        UIUtils::correctAspectRatioVectorX(
                mSystemsDI.getSystem<UIManager>()->getWindow(), 
                cameraGameObject->mTransform->getLocalRotationMatrix().mulVector(Maths::Vector4(mAxis.getEnd(), 1))
            )
        , 1);

    mPositive->mTransform->setLocalPosition(startGlyph);
    mNegative->mTransform->setLocalPosition(endGlyph);
}

void UISingleAxisGizmo::onDestroy()
{
    mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mPositive);
    mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mNegative);
    GameObject::onDestroy();
}

void UIAxisGizmo::createAxis()
{
    Maths::Vector3 axisHalfSize(0.065, 0.065, 0.065);
    Maths::Line axisX = Maths::Line(Maths::Vector3(1,0,0)*axisHalfSize, Maths::Vector3(-1,0,0)*axisHalfSize);
    Maths::Line axisY = Maths::Line(Maths::Vector3(0,1,0)*axisHalfSize, Maths::Vector3(0,-1,0)*axisHalfSize);
    Maths::Line axisZ = Maths::Line(Maths::Vector3(0,0,1)*axisHalfSize, Maths::Vector3(0,0,-1)*axisHalfSize);

    mAxisX = mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UISingleAxisGizmo>();
    mAxisY = mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UISingleAxisGizmo>();
    mAxisZ = mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UISingleAxisGizmo>();

    mAxisX->getSystemsDI().addSystem(mSystemsDI.getSystem<UIManager>());
    mAxisY->getSystemsDI().addSystem(mSystemsDI.getSystem<UIManager>());
    mAxisZ->getSystemsDI().addSystem(mSystemsDI.getSystem<UIManager>());

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
    mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mAxisX);
    mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mAxisY);
    mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mAxisZ);
    GameObject::onDestroy();
}