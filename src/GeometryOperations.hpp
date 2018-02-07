#pragma once

#include "AABB.hpp"
#include "Plane.hpp"
#include "Edge.hpp"

#include <vector>

enum class EdgeSilhouetness : int
{
	EDGE_NOT_SILHOUETTE = 0,
	EDGE_POTENTIALLY_SILHOUETTE = 1,
	EDGE_IS_SILHOUETTE_PLUS = 2,
	EDGE_IS_SILHOUETTE_MINUS = 3
};

#define EDGE_IS_SILHOUETTE(sihlouettness) (int(sihlouettness) > 2)

enum class TestResult : int
{
	ABOVE_OUTSIDE = 1,
	BELOW_INSIDE = -1,
	INTERSECTS_ON = 0
};

namespace GeometryOps
{
	float testPlanePoint(const Plane& plane, const glm::vec3& point);

	bool testAabbPointIsInsideOrOn(const AABB& bbox, const glm::vec3& point);

	TestResult testAabbPlane(const AABB& bbox, const Plane& plane);

	TestResult interpretResult(float result);

	EdgeSilhouetness testEdgeSpaceAabb(const Plane& p1, const Plane& p2, const EDGE_TYPE& edgeInfo, const AABB& voxel);

	int calcEdgeMultiplicity(const EDGE_TYPE& edgeInfo, const glm::vec3& lightPos);

	void buildEdgeTrianglePlane(const Edge& edge, const glm::vec4& oppositeVertex, Plane& plane);
};