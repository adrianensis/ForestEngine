#include "Graphics/Light/Light.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Window/WindowManager.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

Maths::Matrix4 Light::getLightProjectionViewMatrix() const
{
    Maths::Matrix4 lightViewMatrix;
    lightViewMatrix = ECManager.getFirstComponent<Transform>(getOwnerEntity())->getViewMatrix();
    lightViewMatrix.invert();

    Maths::Matrix4 lightProjectionViewMatrix;
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
    // const Maths::Matrix4& modelMatrix = getOwnerEntity()->getFirstComponent<Transform>()->calculateModelMatrix();
    PointLightData data = mLightData;
    // data.mPosition = modelMatrix.mulVector(Maths::Vector4(data.mPosition, 1));

    return data;
}

void DirectionalLight::init(const DirectionalLightData& data)
{
    mLightData = data;
}

DirectionalLightData DirectionalLight::calculateLightData() const
{
    DirectionalLightData data = mLightData;
    const Maths::Matrix4& rotationMatrix = ECManager.getFirstComponent<Transform>(getOwnerEntity())->getLocalRotationMatrix();
    data.mDirection = rotationMatrix.mulVector(Maths::Vector4(-Maths::Vector3::smForward, 1));
    // data.mDirection += getOwnerEntity()->getFirstComponent<Transform>()->getWorldPosition();
    data.mDirection.mul(-1);
    data.mDirection.nor();
    return data;
}