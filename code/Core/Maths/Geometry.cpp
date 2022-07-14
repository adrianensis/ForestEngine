#include "Core/Maths/Geometry.hpp"
#include "Core/Maths/MathUtils.hpp"


Rectangle::Rectangle(CR(Cube) cube)
{
    set(cube.getLeftTopFront(), cube.getSize());
}

Cube::Cube(CR(Rectangle) rectangle)
{
    set(rectangle.getLeftTopFront(), rectangle.getSize());
}

bool Geometry::testCubeSphere(CR(Cube) cube, CR(Sphere) sphere, f32 eps)
{
    return
        (cube.getLeftTopFront().x - sphere.getRadius() - eps) <= sphere.getCenter().x &&
        (cube.getLeftTopFront().x + cube.getSize().x + sphere.getRadius() + eps) >= sphere.getCenter().x &&
        (cube.getLeftTopFront().y + sphere.getRadius() + eps) >= sphere.getCenter().y &&
        (cube.getLeftTopFront().y - cube.getSize().y - sphere.getRadius() - eps) <= sphere.getCenter().y &&
        (cube.getLeftTopFront().z + sphere.getRadius() + eps) >= sphere.getCenter().z &&
        (cube.getLeftTopFront().z - cube.getSize().z - sphere.getRadius() - eps) <= sphere.getCenter().z;
}

bool Geometry::testCubePoint(CR(Cube) cube, CR(Vector3) point, f32 eps)
{
    return testCubeSphere(cube, Sphere(point, 0.0f), eps);
}

bool Geometry::testSphereSphere(CR(Sphere) sphereA, CR(Sphere) sphereB, f32 eps)
{
    f32 distance = sphereA.getCenter().dst(sphereB.getCenter());
    return (distance < (sphereA.getRadius() + sphereB.getRadius() + eps));
}

bool Geometry::testLineLine(CR(Line) lineA, CR(Line) lineB, R(Vector3) intersectionResult, f32 eps)
{

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

bool Geometry::testLineSphereSimple(CR(Line) line, CR(Sphere) sphere, f32 eps)
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

u8 Geometry::testLineSphere(CR(Line) line, CR(Sphere) sphere, f32 eps, R(Vector3) intersectionResult1, R(Vector3) intersectionResult2)
{


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

        f32 t = -B/(2*A);

        intersectionResult1.x = line.getStart().x + t*(dVector.x);
        intersectionResult1.y = line.getStart().y + t*(dVector.y);
        intersectionResult1.z = line.getStart().z + t*(dVector.z);

        return 1;
    }

    if ( det > MathUtils::FLOAT_EPSILON )
    {

        f32 t1 = (-B + std::sqrt( (B*B) - 4.0f*A*C )) / (2.0f*A);

        intersectionResult1.x = line.getStart().x + t1*(dVector.x);
        intersectionResult1.y = line.getStart().y + t1*(dVector.y);
        intersectionResult1.z = line.getStart().z + t1*(dVector.z);
        
        f32 t2 = (-B - std::sqrt((B*B) - 4.0f*A*C )) / (2.0f*A);

        intersectionResult2.x = line.getStart().x + t2*(dVector.x);
        intersectionResult2.y = line.getStart().y + t2*(dVector.y);
        intersectionResult2.z = line.getStart().z + t2*(dVector.z);

        return 2;
    }

    return 0;
}

bool Geometry::testSpherePoint(CR(Vector3) point, CR(Sphere) sphere, f32 eps)
{
    return sphere.getCenter().dst(point) <= (sphere.getRadius() + eps);
}

bool Geometry::testLinePoint(CR(Line) line, CR(Vector3) point, f32 eps)
{
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

Vector3 Geometry::closestPointInLine(CR(Line) line, CR(Vector3) point)
{
    Vector3 pointStartVector = (point - line.getStart()) /*.nor()*/;
    Vector3 lineVector = (line.getEnd() - line.getStart()) /*.nor()*/;

    f32 t = pointStartVector.dot(lineVector) / lineVector.dot(lineVector);

    t = std::fmaxf(t, 0.0f); // clamp to 0
    t = std::fminf(t, 1.0f); // clampt to 1

    return Vector3(line.getStart() + (lineVector.mul(t)));
}

Vector3 Geometry::midPoint(CR(Line) line)
{
    return Vector3((line.getStart().x + line.getEnd().x) / 2.0f, (line.getStart().y + line.getEnd().y) / 2.0f, (line.getStart().z + line.getEnd().z) / 2.0f);
}
