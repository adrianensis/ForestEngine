#pragma once

#include "Core/ObjectBase.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/Vector3.hpp"

#ifdef CPP_INCLUDE
#include "Core/Maths/Geometry.hpp"
#include "Core/Maths/MathUtils.hpp"
#endif

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

    CPP static bool testRectanglePoint(const Vector2& leftTop, f32 width, f32 height, const Vector2& point, f32 eps)
    {
        return (leftTop.x - eps <= point.x && leftTop.y + eps >= point.y && leftTop.x + width + eps >= point.x && leftTop.y - height - eps <= point.y);
    }

    CPP static bool testRectangleSphere(const Vector3& leftTop, f32 width, f32 height, f32 length, const Vector3& center, f32 radius, f32 eps)
    {
        return
            (leftTop.x - radius - eps) <= center.x &&
            (leftTop.y + radius + eps) >= center.y &&
            (leftTop.z + radius + eps) >= center.z &&
            (leftTop.x + width + radius + eps) >= center.x &&
            (leftTop.y - height - radius - eps) <= center.y &&
            (leftTop.z - length - radius - eps) <= center.z;
    }

    CPP static bool testSphereSphere(const Vector2& centerA, const Vector2& centerB, f32 radiusA, f32 radiusB, f32 eps)
    {
        f32 distance = centerA.dst(centerB);
        return (distance < (radiusA + radiusB + eps));
    }

    CPP static bool testLineLine(const Vector2& lineAStart, const Vector2& lineAEnd, const Vector2& lineBStart, const Vector2& lineBEnd, Vector2& intersectionResult)
    {
        // Source : http://www-cs.ccny.cuny.edu/~wolberg/capstone/intersection/Intersection%20point%20of%20two%20lines.html

        /*
        Pa = P1 + ua ( P2 - P1 )
        Pb = P3 + ub ( P4 - P3 )

        Pa = Pb
        */

        // calculate the distance to intersection point
        f32 uA = ((lineBEnd.x - lineBStart.x) * (lineAStart.y - lineBStart.y) - (lineBEnd.y - lineBStart.y) * (lineAStart.x - lineBStart.x)) / ((lineBEnd.y - lineBStart.y) * (lineAEnd.x - lineAStart.x) - (lineBEnd.x - lineBStart.x) * (lineAEnd.y - lineAStart.y));
        f32 uB = ((lineAEnd.x - lineAStart.x) * (lineAStart.y - lineBStart.y) - (lineAEnd.y - lineAStart.y) * (lineAStart.x - lineBStart.x)) / ((lineBEnd.y - lineBStart.y) * (lineAEnd.x - lineAStart.x) - (lineBEnd.x - lineBStart.x) * (lineAEnd.y - lineAStart.y));

        bool intersection = false;
        // if uA and uB are between 0-1, lines are colliding
        if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
        {
            intersectionResult.x = lineAStart.x + (uA * (lineAEnd.x - lineAStart.x));
            intersectionResult.y = lineAStart.y + (uA * (lineAEnd.y - lineAStart.y));

            intersection = true;
        }

        return intersection;
    }

    CPP static bool testLineSphereSimple(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& center, f32 radius, f32 eps)
    {
        bool lineIntersectsSphere = false;

        Vector2 closestPoint(closestPointInLine(lineStart, lineEnd, center));

        if (testSpherePoint(lineStart, center, radius + eps) || testSpherePoint(lineEnd, center, radius + eps))
        {
            lineIntersectsSphere = true;
        }
        else
        {
            lineIntersectsSphere = testSpherePoint(closestPoint, center, radius + eps);
        }

        return lineIntersectsSphere;
    }

    CPP static bool testLineSphere(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& center, f32 radius, f32 eps, Vector2& intersectionResult1, Vector2& intersectionResult2)
    {
        // X(t) = x1 + (x2 - x1) * t
        // Y(t) = y1 + (y2 - y1) * t
        //
        // (X - center.x)2 + (Y - center.y)2 = radius2

        f32 radiusEps = radius + eps;

        bool lineIntersectsSphere = false;

        f32 t;

        f32 dx = lineEnd.x - lineStart.x;
        f32 dy = lineEnd.y - lineStart.y;

        Vector2 startToCenter = lineStart - center;

        f32 A = dx * dx + dy * dy;
        f32 B = 2.0f * (dx * startToCenter.x + dy * startToCenter.y);
        f32 C = startToCenter.x * startToCenter.x + startToCenter.y * startToCenter.y - radiusEps * radiusEps;

        f32 det = B * B - 4 * A * C;

        if (det >= 0)
        {
            // Two solutions.
            f32 sqrtDet = sqrtf(det);

            t = (f32)((-B + sqrtDet) / (2 * A));
            intersectionResult1 = Vector2(lineStart.x + t * dx, lineStart.y + t * dy);

            t = (f32)((-B - sqrtDet) / (2 * A));
            intersectionResult2 = Vector2(lineStart.x + t * dx, lineStart.y + t * dy);

            lineIntersectsSphere = true;
        }

        return lineIntersectsSphere;
    }

    CPP static bool testSpherePoint(const Vector2& point, const Vector2& center, f32 radius)
    {
        return center.dst(point) <= radius;
    }

    CPP static bool testLinePoint(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& point, f32 eps)
    {
        // get distance from the point to the two ends of the line
        f32 d1 = lineStart.dst(point);
        f32 d2 = lineEnd.dst(point);

        f32 lineLen = lineStart.dst(lineEnd);

        bool pointIsInLine = false;

        if (d1 + d2 >= lineLen - eps && d1 + d2 <= lineLen + eps)
        {
            pointIsInLine = true;
        }

        return pointIsInLine;
    }

    CPP static Vector2 closestPointInLine(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& point)
    {
        Vector2 pointStartVector = (point - lineStart) /*.nor()*/;
        Vector2 lineVector = (lineEnd - lineStart) /*.nor()*/;

        f32 t = pointStartVector.dot(lineVector) / lineVector.dot(lineVector);

        t = std::fmaxf(t, 0.0f); // clamp to 0
        t = std::fminf(t, 1.0f); // clampt to 1

        return Vector2(lineStart + (lineVector.mul(t)));
    }

    CPP static Vector2 midPoint(const Vector2& a, const Vector2& b)
    {
        return Vector2((a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f);
    }

};