#include "EdgeExtractor.hpp"
#include "OctreeVisitor.hpp"

void EdgeExtractor::extractEdgesFromTriangles(const std::vector<Triangle>& triangles, EDGE_CONTAINER_TYPE& edges) const
{
	std::map<Edge, std::vector<glm::vec4>> tmpMap;
	
	for (const auto t : triangles)
	{
		bool isCCW;

		auto e1 = Edge(t.v1, t.v2, isCCW);
		tmpMap[e1].push_back(_encodeEdgeWindingInTriangleToOppositeVertex(t.v3, isCCW));

		auto e2 = Edge(t.v2, t.v3, isCCW);
		tmpMap[e2].push_back(_encodeEdgeWindingInTriangleToOppositeVertex(t.v1, isCCW));

		auto e3 = Edge(t.v3, t.v1, isCCW);
		tmpMap[e3].push_back(_encodeEdgeWindingInTriangleToOppositeVertex(t.v2, isCCW));
	}

	edges.reserve(tmpMap.size());

	for(const auto edge : tmpMap)
		edges.push_back(edge);
}

glm::vec4 EdgeExtractor::_encodeEdgeWindingInTriangleToOppositeVertex(const glm::vec4& v, bool isCCW) const
{
	glm::vec4 r = v;

	if (isCCW)
		r.w = 1.0f;
	else
		r.w = -1.0f;

	return r;
}

bool EdgeExtractor::decodeEdgeWindingIsCCW(const glm::vec4& oppositeVertex)
{
	return oppositeVertex.w > 0;
}