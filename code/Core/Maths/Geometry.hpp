#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "Core/ObjectBase.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Log/Log.hpp"

class Shape: public ObjectBase
{
    GENERATE_METADATA(Shape)
    
protected: 
    u32 mVerticesCount = 0;

public:
    void serialize(JSON& json) const override { }
	void deserialize(const JSON& json) override { }

    COPY(Shape)
    {
        DO_COPY(mVerticesCount)
    }

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

    COPY(Line)
    {
        Shape::copy(other);
        DO_COPY(mStart)
        DO_COPY(mEnd)
    }

    void serialize(JSON& json) const override
    {
        Shape::serialize(json);

        DO_SERIALIZE("start", mStart)
        DO_SERIALIZE("end", mEnd)
    }

    void deserialize(const JSON& json) override
    {
        Shape::deserialize(json);

        DO_DESERIALIZE("start", mStart)
        DO_DESERIALIZE("end", mEnd)
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

    void set(const Vector3& leftTopFront, const Vector3& size)
    {
        mLeftTopFront.set(leftTopFront);
        mSize.set(size);
    }

    COPY(Rectangle)
    {
        Shape::copy(other);
        DO_COPY(mLeftTopFront)
        DO_COPY(mSize)
    }

    void serialize(JSON& json) const override
    {
        Shape::serialize(json);

        DO_SERIALIZE("left_top", mLeftTopFront)
        DO_SERIALIZE("size", mSize)
    }

    void deserialize(const JSON& json) override
    {
        Shape::deserialize(json);

        DO_DESERIALIZE("left_top", mLeftTopFront);
        DO_DESERIALIZE("size", mSize);
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

    COPY(Sphere)
    {
        Shape::copy(other);
        DO_COPY(mCenter)
        DO_COPY(mRadius)
    }

    CRGET_SET(Center)
    CRGET_SET(Radius)
};

class Geometry
{
public:

    static bool testCubeSphere(const Cube& cube, const Sphere& sphere, f32 eps);
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

#endif