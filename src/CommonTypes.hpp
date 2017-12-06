#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "AABB.hpp"

#define TEST_RESULT_ABOVE 1
#define TEST_RESULT_BELOW -1
#define TEST_RESULT_INTERSECTS 0

struct Triangle
{
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 v3;
};

#define EDGE_NOT_SILHOUETTE 0
#define EDGE_POTENTIALLY_SILHOUETTE 1
#define EDGE_IS_SILHOUETTE_PLUS 2
#define EDGE_IS_SILHOUETTE_MINUS 3

#define EDGE_IS_SILHOUETTE(bitCode) (bitCode >=2)

struct Edge
{
	glm::vec3 lowerPoint;
	glm::vec3 higherPoint;

	Edge(const glm::vec4& v1, const glm::vec4& v2, bool& isCCW)
	{
		//If swap occurs, that means 
		//the stored form of the edge is CW and not CCW
		//with respect to the triangle it came from
		if (_lessThan(v1, v2))
		{
			lowerPoint = glm::vec3(v1);
			higherPoint = glm::vec3(v2);
			isCCW = true;
		}
		else
		{
			lowerPoint = glm::vec3(v2);
			higherPoint = glm::vec3(v1);
			isCCW = false;
		}
	}

	bool operator<(const Edge& other) const
	{
		const glm::vec3 center1 = lowerPoint + (higherPoint - lowerPoint)/2.0f;
		const glm::vec3 center2 = other.lowerPoint + (other.higherPoint - other.lowerPoint) / 2.0f;

		return _lessThan(center1, center2);
	}

private:
	bool _lessThan(const glm::vec4& v1, const glm::vec4& v2) const
	{
		return _lessThan(glm::vec3(v1), glm::vec3(v2));
	}

	bool _lessThan(const glm::vec3& v1, const glm::vec3& v2) const
	{
		if (v1.x < v2.x)
			return true;
		else if (v1.x > v2.x)
			return false;
		else //else v1.x == v2.x
		{
			if (v1.y < v2.y)
				return true;
			else if (v1.y > v2.y)
				return false;
			else
			{
				/*if(v1z < v2.z)
				return true;
				else if(v1.z >= v2.z)
				return false;*/
				return v1.z < v2.z;
			}
		}
	}
};

class Plane
{
public:
	glm::vec4 equation;

	float testPoint(const glm::vec3& point) const
	{
		return equation.x*point.x + equation.y*point.y + equation.z*point.z + equation.w;
	}

	int testAABB(const AABB& bbox) const
	{
		glm::vec3 points[8];
		bbox.getAllVertices(points);

		const int result = _interpretTestResult(testPoint(points[0]));

		for (unsigned int i = 1; i < 8; ++i)
		{
			int r = _interpretTestResult(testPoint(points[i]));

			if (r != result)
				return TEST_RESULT_INTERSECTS;
		}

		return result;
	}

	void createFromPointsCCW(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
	{
		glm::vec3 normal = glm::normalize(glm::cross(glm::normalize(v3 - v2), glm::normalize(v1 - v2)));

		createFromPointNormalCCW(v2, normal);
	}

	void createFromPointNormalCCW(const glm::vec3& point, const glm::vec3& normal)
	{
		equation = glm::vec4(normal, -point.x*normal.x - point.y*normal.y - point.z*normal.z);
	}

private:
	int _interpretTestResult(float result) const
	{
		if (result > 0)
			return TEST_RESULT_ABOVE;
		else
			return TEST_RESULT_BELOW;
	}
};
