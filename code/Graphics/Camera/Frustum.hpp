#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include "Core/Module.hpp"


class Camera;

class Frustum: public ObjectBase
{
    GENERATE_METADATA(Frustum)

public:
    void init(Camera *camera);
    bool testSphere(CR(Vector3) center, f32 radius) const;
    bool testPoint(CR(Vector3) point) const;
    bool testRectangle(CR(Vector3) leftTop, f32 width, f32 height) const;
    void build();
	
private: 
	Matrix4 mVPmatrix;
	Camera* mCamera = nullptr;
	std::array<Vector4, 6> mPlanes;
};

#endif