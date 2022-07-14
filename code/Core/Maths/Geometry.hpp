#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "Core/ObjectBase.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/MathUtils.hpp"
#include "Core/Log/Log.hpp"

class Shape: public ObjectBase
{
    GENERATE_METADATA(Shape)
    
protected: 
    u32 mVerticesCount = 0;

public:

    virtual bool isZero() const = 0;

    void serialize(R(JSON) json) const override { }
	void deserialize(CR(JSON) json) override { }

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

    Line(CR(Vector3) start, CR(Vector3) end): Line()
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

    COPY(Line)
    {
        Shape::copy(other);
        DO_COPY(mStart)
        DO_COPY(mEnd)
    }

    void serialize(R(JSON) json) const override
    {
        Shape::serialize(json);

        SERIALIZE("start", mStart)
        SERIALIZE("end", mEnd)
    }

    void deserialize(CR(JSON) json) override
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

    Rectangle(CR(Vector3) leftTopFront, CR(Vector3) size): Rectangle()
    {
        set(leftTopFront, size);
    }

    Rectangle(CR(Cube) cube);

    void set(CR(Vector3) leftTopFront, CR(Vector3) size)
    {
        mLeftTopFront.set(leftTopFront);
        mSize.set(size);
    }

    bool isZero() const override
    {
        return getSize().len() <= MathUtils::FLOAT_EPSILON;
    }

    COPY(Rectangle)
    {
        Shape::copy(other);
        DO_COPY(mLeftTopFront)
        DO_COPY(mSize)
    }

    void serialize(R(JSON) json) const override
    {
        Shape::serialize(json);

        SERIALIZE("left_top", mLeftTopFront)
        SERIALIZE("size", mSize)
    }

    void deserialize(CR(JSON) json) override
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

    Cube(CR(Vector3) leftTopFront, CR(Vector3) size): Cube()
    {
        set(leftTopFront, size);
    }

    Cube(CR(Rectangle) rectangle);
};

class Sphere: public Shape
{
    GENERATE_METADATA(Sphere)

protected: 
    Vector3 mCenter;
    f32 mRadius;

public:
    Sphere() { mVerticesCount = 0; }

    Sphere(CR(Vector3) center, f32 radius): Sphere()
    {
        mCenter.set(center);
        mRadius = radius;
    }

    bool isZero() const override
    {
        return mRadius <= MathUtils::FLOAT_EPSILON;
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

    static bool testCubeSphere(CR(Cube) cube, CR(Sphere) sphere, f32 eps);
    static bool testCubePoint(CR(Cube) cube, CR(Vector3) point, f32 eps);
    static bool testSphereSphere(CR(Sphere) sphereA, CR(Sphere) sphereB, f32 eps);
    static bool testLineLine(CR(Line) lineA, CR(Line) lineB, R(Vector3) intersectionResult, f32 eps);
    static bool testLineSphereSimple(CR(Line) line, CR(Sphere) sphere, f32 eps);
    static u8 testLineSphere(CR(Line) line, CR(Sphere) sphere, f32 eps, R(Vector3) intersectionResult1, R(Vector3) intersectionResult2);
    static bool testSpherePoint(CR(Vector3) point, CR(Sphere) sphere, f32 eps);
    static bool testLinePoint(CR(Line) line, CR(Vector3) point, f32 eps);
    static Vector3 closestPointInLine(CR(Line) line, CR(Vector3) point);
    static Vector3 midPoint(CR(Line) line);
};

#endif