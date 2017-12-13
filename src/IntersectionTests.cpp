#include "IntersectionTests.hpp"

bool isInRange(float value, float min, float max)
{
	return value >= min && value <= max;
}


float IntersectTest::testPlanePoint(const Plane& plane, const glm::vec3& point)
{
	return plane.equation.x*point.x + plane.equation.y*point.y + plane.equation.z*point.z + plane.equation.w;
}

TestResult IntersectTest::testAabbPlane(const AABB& bbox, const Plane& plane)
{
	glm::vec3 points[8];
	bbox.getAllVertices(points);

	const TestResult result = interpretResult(testPlanePoint(plane, points[0]));

	for (unsigned int i = 1; i < 8; ++i)
	{
		TestResult r = interpretResult(testPlanePoint(plane, points[i]));

		if (r != result)
			return TestResult::INTERSECTS_ON;
	}

	return result;
}

TestResult IntersectTest::interpretResult(float result)
{
	TestResult r = TestResult::INTERSECTS_ON;

	if (result > 0)
		r = TestResult::ABOVE_OUTSIDE;
	else if (result < 0)
		r = TestResult::BELOW_INSIDE;

	return r;
}

bool IntersectTest::testAabbPointIsInsideOrOn(const AABB& bbox, const glm::vec3& point)
{
	const auto minPoint = bbox.getMinPoint();
	const auto maxPoint = bbox.getMaxPoint();
	
	return isInRange(point.x, minPoint.x, maxPoint.x) & isInRange(point.y, minPoint.y, maxPoint.y) & isInRange(point.z, minPoint.z, maxPoint.z);
}

