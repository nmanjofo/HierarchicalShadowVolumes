#include "GeometryOperations.hpp"

#include "EdgeExtractor.hpp"

inline bool isInRange(float value, float min, float max)
{
	return value >= min && value <= max;
}

float GeometryOps::testPlanePoint(const Plane& plane, const glm::vec3& point)
{
	return plane.equation.x*point.x + plane.equation.y*point.y + plane.equation.z*point.z + plane.equation.w;
}

TestResult GeometryOps::testAabbPlane(const AABB& bbox, const Plane& plane)
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

TestResult GeometryOps::interpretResult(float result)
{
	TestResult r = TestResult::INTERSECTS_ON;

	if (result > 0)
		r = TestResult::ABOVE_OUTSIDE;
	else if (result < 0)
		r = TestResult::BELOW_INSIDE;

	return r;
}

bool GeometryOps::testAabbPointIsInsideOrOn(const AABB& bbox, const glm::vec3& point)
{
	const auto minPoint = bbox.getMinPoint();
	const auto maxPoint = bbox.getMaxPoint();
	
	return isInRange(point.x, minPoint.x, maxPoint.x) & isInRange(point.y, minPoint.y, maxPoint.y) & isInRange(point.z, minPoint.z, maxPoint.z);
}

EdgeSilhouetness GeometryOps::testEdgeSpaceAabb(const Plane& p1, const Plane& p2, const EDGE_TYPE& edgeInfo, const AABB& voxel)
{
	auto result1 = testAabbPlane(voxel, p1);
	auto result2 = testAabbPlane(voxel, p2);

	EdgeSilhouetness result = EdgeSilhouetness::EDGE_NOT_SILHOUETTE;

	if (result1 == TestResult::INTERSECTS_ON || result2 == TestResult::INTERSECTS_ON)
		result = EdgeSilhouetness::EDGE_POTENTIALLY_SILHOUETTE;
	else if ((int(result1)*int(result2)) < 0)
	{
		int multiplicity = calcEdgeMultiplicity(edgeInfo, voxel.getMinPoint());

		if (multiplicity >= 0)
			result = EdgeSilhouetness::EDGE_IS_SILHOUETTE_PLUS;
		else
			result = EdgeSilhouetness::EDGE_IS_SILHOUETTE_MINUS;
	}

	return result;
}

int GeometryOps::calcEdgeMultiplicity(const EDGE_TYPE& edgeInfo, const glm::vec3& lightPos)
{
	const auto& edge = edgeInfo.first;
	const auto& oppositeVertices = edgeInfo.second;

	Plane lightPlane;
	lightPlane.createFromPointsCCW(edge.lowerPoint, edge.higherPoint, lightPos);
	int multiplicity = 0;
	for (const auto& oppositeVertex : oppositeVertices)
	{
		const float r = GeometryOps::testPlanePoint(lightPlane, glm::vec3(oppositeVertex));
		multiplicity += (r > 0) - (r < 0);
	}

	return multiplicity;
}

void GeometryOps::buildEdgeTrianglePlane(const Edge& edge, const glm::vec4& oppositeVertex, Plane& plane)
{
	const bool isCCW = EdgeExtractor::decodeEdgeWindingIsCCW(oppositeVertex);

	if (isCCW)
		plane.createFromPointsCCW(edge.lowerPoint, edge.higherPoint, glm::vec3(oppositeVertex));
	else
		plane.createFromPointsCCW(edge.higherPoint, edge.lowerPoint, glm::vec3(oppositeVertex));
}