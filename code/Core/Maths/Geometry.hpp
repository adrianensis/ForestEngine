#pragma once

#include "Core/ObjectBase.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/Vector3.hpp"

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

protected: 
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

// class Rectangle: public Shape
// {
//     GENERATE_METADATA(Rectangle)
//     protected: Vector3 mLeftTop; CRGET_SET(LeftTop)
//     protected: Vector3 mSize; CRGET_SET(Size)

// public:
//     Rectangle() { mVerticesCount = 8; }

//     Rectangle(f32 x, f32 y, f32 z, f32 w, f32 h, f32 l): Rectangle()
//     {
//         mLeftTop.set(x,y,z);
//         mSize.set(w,h,l);
//     }

//     Rectangle(const Vector3& leftTop, f32 w, f32 h, f32 l): Rectangle()
//     {
//         mLeftTop.set(leftTop);
//         mSize.set(w,h,l);
//     }

//     Rectangle(const Vector3& leftTop, const Vector3& size): Rectangle()
//     {
//         mLeftTop.set(leftTop);
//         mSize.set(size);
//     }

//     COPY(Rectangle)
//     {
//         Shape::copy(other);
//         DO_COPY(mLeftTop)
//         DO_COPY(mSize)
//     }

//     void serialize(JSON& json) const override
//     {
//         Shape::serialize(json);

//         DO_SERIALIZE("left_top", mLeftTop)
//         DO_SERIALIZE("size", mSize)
//     }

//     void deserialize(const JSON& json) override
//     {
//         Shape::deserialize(json);

//         DO_DESERIALIZE("left_top", mLeftTop);
//         DO_DESERIALIZE("size", mSize);
//     }
// };

class Rectangle: public Shape
{
    GENERATE_METADATA(Rectangle)

protected: 
    Vector3 mLeftTop;
    Vector2 mSize;

public:
    Rectangle() { mVerticesCount = 4; }

    Rectangle(f32 x, f32 y, f32 w, f32 h): Rectangle()
    {
        mLeftTop.set(x,y,0);
        mSize.set(w,h);
    }

    Rectangle(f32 x, f32 y, f32 z, f32 w, f32 h): Rectangle()
    {
        mLeftTop.set(x,y,z);
        mSize.set(w,h);
    }

    Rectangle(const Vector3& leftTop, f32 w, f32 h): Rectangle()
    {
        mLeftTop.set(leftTop);
        mSize.set(w,h);
    }

    Rectangle(const Vector3& leftTop, const Vector2& size): Rectangle()
    {
        mLeftTop.set(leftTop);
        mSize.set(size);
    }

    COPY(Rectangle)
    {
        Shape::copy(other);
        DO_COPY(mLeftTop)
        DO_COPY(mSize)
    }

    void serialize(JSON& json) const override
    {
        Shape::serialize(json);

        DO_SERIALIZE("left_top", mLeftTop)
        DO_SERIALIZE("size", mSize)
    }

    void deserialize(const JSON& json) override
    {
        Shape::deserialize(json);

        DO_DESERIALIZE("left_top", mLeftTop);
        DO_DESERIALIZE("size", mSize);
    }

    CRGET_SET(LeftTop)
    CRGET_SET(Size)
};

class Geometry
{
public:
    // Geometry tests
    static bool testRectanglePoint(const Vector2& leftTop, f32 width, f32 height, const Vector2& point, f32 eps);
    static bool testRectangleSphere(const Vector3& leftTop, f32 width, f32 height, f32 length, const Vector3& center, f32 radius, f32 eps);
    static bool testSphereSphere(const Vector2& centerA, const Vector2& centerB, f32 radiusA, f32 radiusB, f32 eps);
    static bool testLineLine(const Vector2& lineAStart, const Vector2& lineAEnd, const Vector2& lineBStart, const Vector2& lineBEnd, Vector2& intersectionResult);
    static bool testLineSphereSimple(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& center, f32 radius, f32 eps);
    static bool testLineSphere(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& center, f32 radius, f32 eps, Vector2& intersectionResult1, Vector2& intersectionResult2);
    static bool testSpherePoint(const Vector2& point, const Vector2& center, f32 radius);
    static bool testLinePoint(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& point, f32 eps);
    static Vector2 closestPointInLine(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& point);
    static Vector2 midPoint(const Vector2& a, const Vector2& b);
};