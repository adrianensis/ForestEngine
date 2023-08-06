#pragma once

#include "Core/ObjectBase.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/MathUtils.hpp"
#include "Core/Log/Log.hpp"

class Cube;
class Sphere;
class Line;

class Geometry
{
public:

    static bool testCubeSphere(const Cube& cube, const Sphere& sphere, f32 eps);
    static bool testSphereInsideCube(const Cube& cube, const Sphere& sphere);
    static bool testCubePoint(const Cube& cube, const Vector3& point, f32 eps);
    static bool testSphereSphere(const Sphere& sphereA, const Sphere& sphereB, f32 eps);
    static bool testLineLine(const Line& lineA, const Line& lineB, Vector3& intersectionResult, f32 eps);
    static bool testLineSphereSimple(const Line& line, const Sphere& sphere, f32 eps);
    static u8 testLineSphere(const Line& line, const Sphere& sphere, f32 eps, Vector3& intersectionResult1, Vector3& intersectionResult2);
    static bool testSpherePoint(const Vector3& point, const Sphere& sphere, f32 eps);
    static bool testLinePoint(const Line& line, const Vector3& point, f32 eps);
    static Vector3 closestPointInLine(const Line& line, const Vector3& point);
    static Vector3 midPoint(const Line& line);
};

class Face
{
	GENERATE_METADATA_STRUCT(Face);
	
public:
    Face(u32 index0, u32 index1, u32 index2);

public:
	u32 mIndex0 = 0;
    u32 mIndex1 = 0;
    u32 mIndex2 = 0;
};

class Shape: public ObjectBase
{
    GENERATE_METADATA(Shape)
    
protected: 
    u32 mVerticesCount = 0;

public:

    virtual bool isZero() const { return true; };
    virtual Vector3 getCenter() const { return Vector3(); }
    void serialize(JSON& json) const override { }
	void deserialize(const JSON& json) override { }

    GET(VerticesCount)
};

class Line: public Shape
{
    GENERATE_METADATA(Line)

private: 
    Vector3 mStart;
    Vector3 mEnd;

public:
    Line() { mVerticesCount = 2; }

    Line(f32 xStart, f32 yStart, f32 xEnd, f32 yEnd): Line()
    {
        mStart.set(xStart,yStart, 0);
        mEnd.set(xEnd, yEnd, 0);
    }

    Line(const Vector3& start, const Vector3& end): Line()
    {
        mStart.set(start);
        mEnd.set(end);
    }

    Vector3 toVector() const
    {
        return mEnd - mStart;
    }

    bool isZero() const override
    {
        return toVector().len() <= MathUtils::FLOAT_EPSILON;
    }

    Vector3 getCenter() const override
    {
        return Geometry::midPoint(*this);
    }
    void serialize(JSON& json) const override
    {
        Shape::serialize(json);

        SERIALIZE("start", mStart)
        SERIALIZE("end", mEnd)
    }

    void deserialize(const JSON& json) override
    {
        Shape::deserialize(json);

        DESERIALIZE("start", mStart)
        DESERIALIZE("end", mEnd)
    }

    CRGET_SET(Start)
    CRGET_SET(End)
};

class Cube;

class Rectangle: public Shape
{
    GENERATE_METADATA(Rectangle)

protected: 
    Vector3 mLeftTopFront;
    Vector3 mSize;

public:
    Rectangle() { mVerticesCount = 4; }

    Rectangle(const Vector3& leftTopFront, const Vector3& size): Rectangle()
    {
        set(leftTopFront, size);
    }

    Rectangle(const Cube& cube);

    virtual void set(const Vector3& leftTopFront, const Vector3& size)
    {
        mLeftTopFront.set(leftTopFront);
        mSize.set(Vector2(size));
    }

    bool isZero() const override
    {
        return getSize().len() <= MathUtils::FLOAT_EPSILON;
    }

    Vector3 getCenter() const override
    {
        return mLeftTopFront - (Vector3(-mSize.x, mSize.y, mSize.z)/2.0f);
    }

    void serialize(JSON& json) const override
    {
        Shape::serialize(json);

        SERIALIZE("left_top", mLeftTopFront)
        SERIALIZE("size", mSize)
    }

    void deserialize(const JSON& json) override
    {
        Shape::deserialize(json);

        DESERIALIZE("left_top", mLeftTopFront);
        DESERIALIZE("size", mSize);
    }

    CRGET_SET(LeftTopFront)
    CRGET_SET(Size)
};

class Cube: public Rectangle
{
    GENERATE_METADATA(Cube)

public:
    Cube() { mVerticesCount = 8; }

    Cube(const Vector3& leftTopFront, const Vector3& size): Cube()
    {
        set(leftTopFront, size);
    }

    Cube(const Rectangle& rectangle);

    void set(const Vector3& leftTopFront, const Vector3& size) override
    {
        mLeftTopFront.set(leftTopFront);
        mSize.set(size);
    }
};

class Sphere: public Shape
{
    GENERATE_METADATA(Sphere)

protected: 
    Vector3 mCenter;
    f32 mRadius;

public:
    Sphere() { mVerticesCount = 0; }

    Sphere(const Vector3& center, f32 radius): Sphere()
    {
        mCenter.set(center);
        mRadius = radius;
    }

    bool isZero() const override
    {
        return mRadius <= MathUtils::FLOAT_EPSILON;
    }

    Vector3 getCenter() const override
    {
        return mCenter;
    }

    CRGET_SET(Radius)
};