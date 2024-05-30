#include "Graphics/Light/Light.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/Window/Window.hpp"
#include "Scene/GameObject.hpp"

ClassId Light::getSystemComponentId() const { return ClassManager::getClassMetadata<Light>().mClassDefinition.getId(); }

Matrix4 Light::getLightProjectionViewMatrix() const
{
    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    const Matrix4& lightViewMatrix = mGameObject->mTransform->getViewMatrix();

    Vector2 windowSize = GET_SYSTEM(Window).getWindowSize();
    Matrix4 lightProjectionViewMatrix;
    lightProjectionViewMatrix.ortho(-windowSize.x, windowSize.x, -windowSize.y, windowSize.y, 1.0, 10000);

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