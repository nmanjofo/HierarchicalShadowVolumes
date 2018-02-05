#pragma once

#include "Triangle.hpp"
#include "Edge.hpp"

#include <vector>
#include <map>

class EdgeExtractor
{
public:

	void extractEdgesFromTriangles(const std::vector<Triangle>& triangles, std::vector<std::pair<Edge, std::vector<glm::vec4>>>& edges) const;

	static bool decodeEdgeWindingIsCCW(const glm::vec4& oppositeVertex);

private:
	glm::vec4 _encodeEdgeWindingInTriangleToOppositeVertex(const glm::vec4& v, bool isCCW) const;

};
