#include "Edge.hpp"

Edge::Edge(const glm::vec4& v1, const glm::vec4& v2, bool& isCCW)
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

bool Edge::operator<(const Edge& other) const
{
	const glm::vec3 center1 = lowerPoint + (higherPoint - lowerPoint) / 2.0f;
	const glm::vec3 center2 = other.lowerPoint + (other.higherPoint - other.lowerPoint) / 2.0f;

	return _lessThan(center1, center2);
}

bool Edge::_lessThan(const glm::vec4& v1, const glm::vec4& v2) const
{
	return _lessThan(glm::vec3(v1), glm::vec3(v2));
}

bool Edge::_lessThan(const glm::vec3& v1, const glm::vec3& v2) const
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