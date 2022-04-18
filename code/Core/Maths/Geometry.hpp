#pragma once

#include "Core/ObjectBase.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Log/Log.hpp"

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
    Vector3 mLeftTop;
    Vector3 mSize;

public:
    Rectangle() { mVerticesCount = 4; }

    Rectangle(const Vector3& leftTop, const Vector3& size): Rectangle()
    {
        set(leftTop, size);
    }

    CPP Rectangle(const Cube& cube)
	{
        set(cube.getLeftTop(), cube.getSize());
	}

    void set(const Vector3& leftTop, const Vector3& size)
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

class Cube: public Rectangle
{
    GENERATE_METADATA(Cube)

public:
    Cube() { mVerticesCount = 8; }

    Cube(const Vector3& leftTop, const Vector3& size): Cube()
    {
        set(leftTop, size);
    }

    CPP Cube(const Rectangle& rectangle)
	{
        set(rectangle.getLeftTop(), rectangle.getSize());
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
    // Geometry tests

    CPP static bool testCubeSphere(const Cube& cube, const Sphere& sphere, f32 eps)
    {
        return
            (cube.getLeftTop().x - sphere.getRadius() - eps) <= sphere.getCenter().x &&
            (cube.getLeftTop().x + cube.getSize().x + sphere.getRadius() + eps) >= sphere.getCenter().x &&
            (cube.getLeftTop().y + sphere.getRadius() + eps) >= sphere.getCenter().y &&
            (cube.getLeftTop().y - cube.getSize().y - sphere.getRadius() - eps) <= sphere.getCenter().y &&
            (cube.getLeftTop().z + sphere.getRadius() + eps) >= sphere.getCenter().z &&
            (cube.getLeftTop().z - cube.getSize().z - sphere.getRadius() - eps) <= sphere.getCenter().z;
    }

    CPP static bool testCubePoint(const Cube& cube, const Vector3& point, f32 eps)
    {
        return testCubeSphere(cube, Sphere(point, 0.0f), eps);
    }

    CPP static bool testSphereSphere(const Sphere& sphereA, const Sphere& sphereB, f32 eps)
    {
        f32 distance = sphereA.getCenter().dst(sphereB.getCenter());
        return (distance < (sphereA.getRadius() + sphereB.getRadius() + eps));
    }

    CPP static bool testLineLine(const Line& lineA, const Line& lineB, Vector3& intersectionResult, f32 eps)
    {
        // Source : http://www-cs.ccny.cuny.edu/~wolberg/capstone/intersection/Intersection%20point%20of%20two%20lines.html
        // Source: https://stackoverflow.com/questions/2316490/the-algorithm-to-find-the-point-of-intersection-of-two-3d-line-segment/10288710#10288710
        // Source: https://www.codefull.net/2015/06/intersection-of-a-ray-and-a-line-segment-in-3d/

        Vector3 da = lineA.getEnd() - lineA.getStart(); 
        Vector3 db = lineB.getEnd() - lineB.getStart();
        Vector3 dc = lineB.getStart() - lineA.getStart();

        if (dc.dot(Vector3(da).cross(db)) != 0.0) // lines are not coplanar
        {
            return false;
        }

        f32 s = Vector3(dc).cross(db).dot(Vector3(da).cross(db)) / Vector3(da).cross(db).sqrlen();

        if (s >= 0.0 && s <= 1.0)
        {
            intersectionResult = lineA.getStart() + (da * s);
            return true;
        }

        return false;
    }

    CPP static bool testLineSphereSimple(const Line& line, const Sphere& sphere, f32 eps)
    {
        bool lineIntersectsSphere = false;

        Vector3 closestPoint(closestPointInLine(line, sphere.getCenter()));

        if (testSpherePoint(line.getStart(), sphere, eps) || testSpherePoint(line.getEnd(), sphere, eps))
        {
            lineIntersectsSphere = true;
        }
        else
        {
            lineIntersectsSphere = testSpherePoint(closestPoint, sphere, eps);
        }

        return lineIntersectsSphere;
    }

    CPP static u8 testLineSphere(const Line& line, const Sphere& sphere, f32 eps, Vector3& intersectionResult1, Vector3& intersectionResult2)
    {
        // http://paulbourke.net/geometry/circlesphere/source.cpp

        // X(t) = x1 + (x2 - x1) * t
        // Y(t) = y1 + (y2 - y1) * t
        // (X - center.x)2 + (Y - center.y)2 = radius2

        f32 radiusEps = sphere.getRadius() + eps;

        bool lineIntersectsSphere = false;

        f32 t;

        Vector3 dVector = line.toVector();

        Vector3 startToCenter = line.getStart() - sphere.getCenter();

        f32 A = dVector.sqrlen();
        f32 B = 2.0f * dVector.dot(startToCenter);
        f32 C = startToCenter.sqrlen() - radiusEps * radiusEps;

        f32 det = B * B - 4.0f * A * C;

        if ( det == MathUtils::FLOAT_EPSILON )
        {
            // one intersection

            f32 t = -B/(2*A);

            intersectionResult1.x = line.getStart().x + t*(dVector.x);
            intersectionResult1.y = line.getStart().y + t*(dVector.y);
            intersectionResult1.z = line.getStart().z + t*(dVector.z);

            return 1;
        }

        if ( det > MathUtils::FLOAT_EPSILON )
        {
            // two intersections

            // first intersection
            f32 t1 = (-B + std::sqrt( (B*B) - 4.0f*A*C )) / (2.0f*A);

            intersectionResult1.x = line.getStart().x + t1*(dVector.x);
            intersectionResult1.y = line.getStart().y + t1*(dVector.y);
            intersectionResult1.z = line.getStart().z + t1*(dVector.z);
            
            // second intersection
            f32 t2 = (-B - std::sqrt((B*B) - 4.0f*A*C )) / (2.0f*A);

            intersectionResult2.x = line.getStart().x + t2*(dVector.x);
            intersectionResult2.y = line.getStart().y + t2*(dVector.y);
            intersectionResult2.z = line.getStart().z + t2*(dVector.z);

            return 2;
        }

        // no intersection
        // det < 0.0
        return 0;
    }

    CPP static bool testSpherePoint(const Vector3& point, const Sphere& sphere, f32 eps)
    {
        return sphere.getCenter().dst(point) <= (sphere.getRadius() + eps);
    }

    CPP static bool testLinePoint(const Line& line, const Vector3& point, f32 eps)
    {
        // get distance from the point to the two ends of the line
        f32 d1 = line.getStart().dst(point);
        f32 d2 = line.getEnd().dst(point);

        f32 lineLen = line.getStart().dst(line.getEnd());

        bool pointIsInLine = false;

        if (d1 + d2 >= lineLen - eps && d1 + d2 <= lineLen + eps)
        {
            pointIsInLine = true;
        }

        return pointIsInLine;
    }

    CPP static Vector3 closestPointInLine(const Line& line, const Vector3& point)
    {
        Vector3 pointStartVector = (point - line.getStart()) /*.nor()*/;
        Vector3 lineVector = (line.getEnd() - line.getStart()) /*.nor()*/;

        f32 t = pointStartVector.dot(lineVector) / lineVector.dot(lineVector);

        t = std::fmaxf(t, 0.0f); // clamp to 0
        t = std::fminf(t, 1.0f); // clampt to 1

        return Vector3(line.getStart() + (lineVector.mul(t)));
    }

    CPP static Vector3 midPoint(const Line& line)
    {
        return Vector3((line.getStart().x + line.getEnd().x) / 2.0f, (line.getStart().y + line.getEnd().y) / 2.0f, (line.getStart().z + line.getEnd().z) / 2.0f);
    }

};