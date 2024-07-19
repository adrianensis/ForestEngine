#include "Graphics/Light/Light.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/Window/WindowManager.hpp"
#include "Core/ECS/EntityHandler.hpp"

ClassId Light::getSystemComponentId() const { return ClassManager::getClassMetadata<Light>().mClassDefinition.getId(); }

Matrix4 Light::getLightProjectionViewMatrix() const
{
    TypedComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 lightViewMatrix;
    lightViewMatrix = getOwnerEntity()->getFirstComponent<Transform>()->getViewMatrix();
    lightViewMatrix.invert();

    Matrix4 lightProjectionViewMatrix;
    lightProjectionViewMatrix.ortho(-2048, 2048, -2048, 2048, 1.0, 10000);

    lightProjectionViewMatrix.mul(lightViewMatrix);

    return lightProjectionViewMatrix;
}

void PointLight::init(const PointLightData& data)
{
    mLightData = data;
}

PointLightData PointLight::calculateLightData() const
{
    // const Matrix4& modelMatrix = getOwnerEntity()->getFirstComponent<Transform>()->calculateModelMatrix();
    PointLightData data = mLightData;
    // data.mPosition = modelMatrix.mulVector(Vector4(data.mPosition, 1));

    return data;
}

void DirectionalLight::init(const DirectionalLightData& data)
{
    mLightData = data;
}

DirectionalLightData DirectionalLight::calculateLightData() const
{
    DirectionalLightData data = mLightData;
    const Matrix4& rotationMatrix = getOwnerEntity()->getFirstComponent<Transform>()->getLocalRotationMatrix();
    data.mDirection = rotationMatrix.mulVector(Vector4(-Vector3::smForward, 1));
    // data.mDirection += getOwnerEntity()->getFirstComponent<Transform>()->getWorldPosition();
    data.mDirection.mul(-1);
    data.mDirection.nor();
    return data;
}