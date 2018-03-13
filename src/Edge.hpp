#pragma once

#include <glm/glm.hpp>

#define EDGE_TYPE std::pair<Edge, std::vector<glm::vec4>>
#define EDGE_CONTAINER_TYPE std::vector<EDGE_TYPE>

struct Edge
{
	glm::vec3 lowerPoint;
	glm::vec3 higherPoint;

	Edge(const glm::vec4& v1, const glm::vec4& v2);

	bool operator<(const Edge& other) const;

private:

	bool _lessThan(const glm::vec4& v1, const glm::vec4& v2) const;
	
	bool _lessThan(const glm::vec3& v1, const glm::vec3& v2) const;
};