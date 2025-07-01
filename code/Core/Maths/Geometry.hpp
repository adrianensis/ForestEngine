#pragma once

#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/MathUtils.hpp"

class Cube;
class Sphere;
class Line;

class Geometry
{
public:

    static bool testCubeSphere(const Cube& cube, const Sphere& sphere, Core::f32 eps);
    static bool testSphereInsideCube(const Cube& cube, const Sphere& sphere);
    static bool testCubePoint(const Cube& cube, const Vector3& point, Core::f32 eps);
    static bool testSphereSphere(const Sphere& sphereA, const Sphere& sphereB, Core::f32 eps);
    static bool testLineLine(const Line& lineA, const Line& lineB, Vector3& intersectionResult, Core::f32 eps);
    static bool testLineSphereSimple(const Line& line, const Sphere& sphere, Core::f32 eps);
    static Core::u8 testLineSphere(const Line& line, const Sphere& sphere, Core::f32 eps, Vector3& intersectionResult1, Vector3& intersectionResult2);
    static bool testSpherePoint(const Vector3& point, const Sphere& sphere, Core::f32 eps);
    static bool testLinePoint(const Line& line, const Vector3& point, Core::f32 eps);
    static Vector3 closestPointInLine(const Line& line, const Vector3& point);
    static Vector3 midPoint(const Line& line);
};

class Face
{
public:
    Face(Core::u32 index0, Core::u32 index1, Core::u32 index2);

public:
	Core::u32 mIndex0 = 0;
    Core::u32 mIndex1 = 0;
    Core::u32 mIndex2 = 0;
};

class Line
{
private: 
    Vector3 mStart;
    Vector3 mEnd;

public:
    Line() = default;
    Line(Core::f32 xStart, Core::f32 yStart, Core::f32 xEnd, Core::f32 yEnd)
    {
        mStart.set(xStart,yStart, 0);
        mEnd.set(xEnd, yEnd, 0);
    }

    Line(const Vector3& start, const Vector3& end)
    {
        mStart.set(start);
        mEnd.set(end);
    }

    Vector3 toVector() const
    {
        return mEnd - mStart;
    }

    bool isZero() const
    {
        return toVector().len() <= MathUtils::FLOAT_EPSILON;
    }

    CRGET(Start)
    CRGET(End)
};
REGISTER_CLASS(Line)

class Cube;

class Rectangle
{
private: 
    Vector3 mLeftTopFront;
    Vector3 mSize;

public:
    Rectangle() = default;
    Rectangle(const Vector3& leftTopFront, const Vector3& size)
    {
        set(leftTopFront, size);
    }

    Rectangle(const Cube& cube);

    void set(const Vector3& leftTopFront, const Vector3& size)
    {
        mLeftTopFront.set(leftTopFront);
        mSize.set(Vector2(size));
    }

    Vector3 getCenter() const { return mLeftTopFront - (Vector3(-mSize.x, mSize.y, mSize.z)/2.0f); }
    Core::f32 getRadius() const { return mSize.max(); }

    bool isZero() const
    {
        return getSize().len() <= MathUtils::FLOAT_EPSILON;
    }

    CRGET(LeftTopFront)
    CRGET(Size)
};
REGISTER_CLASS(Rectangle)

class Cube
{
private: 
    Vector3 mLeftTopFront;
    Vector3 mSize;

public:
    Cube() = default;
    Cube(const Vector3& leftTopFront, const Vector3& size)
    {
        set(leftTopFront, size);
    }

    Cube(const Rectangle& rectangle);

    void set(const Vector3& leftTopFront, const Vector3& size)
    {
        mLeftTopFront.set(leftTopFront);
        mSize.set(size);
    }

    Vector3 getCenter() const { return mLeftTopFront - (Vector3(-mSize.x, mSize.y, mSize.z)/2.0f); }
    Core::f32 getRadius() const { return mSize.max(); }

    CRGET(LeftTopFront)
    CRGET(Size)
};
REGISTER_CLASS(Cube)

class Sphere
{
public:
    Sphere() = default;
    Sphere(const Vector3& center, Core::f32 radius)
    {
        mCenter.set(center);
        mRadius = radius;
    }

    bool isZero() const
    {
        return mRadius <= MathUtils::FLOAT_EPSILON;
    }

private:
    Vector3 mCenter;
    Core::f32 mRadius;
public:
    CRGET(Center)
    GET(Radius)
};
REGISTER_CLASS(Sphere)