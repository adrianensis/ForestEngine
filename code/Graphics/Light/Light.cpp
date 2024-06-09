#include "Graphics/Light/Light.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/Window/WindowManager.hpp"
#include "Scene/GameObject.hpp"

ClassId Light::getSystemComponentId() const { return ClassManager::getClassMetadata<Light>().mClassDefinition.getId(); }

Matrix4 Light::getLightProjectionViewMatrix() const
{
    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    const Matrix4& lightViewMatrix = mGameObject->mTransform->getViewMatrix();

    Vector2 windowSize = GET_SYSTEM(WindowManager).getMainWindow()->getWindowSize();
    Matrix4 lightProjectionViewMatrix;
    lightProjectionViewMatrix.ortho(-2048, 2048, -2048, 2048, 1.0, 10000);

    lightProjectionViewMatrix.mul(lightViewMatrix);

    return lightProjectionViewMatrix;
}

void PointLight::init(const PointLightData& data)
{
    mLightData = data;
}

void DirectionalLight::init(const DirectionalLightData& data)
{
    mLightData = data;
}