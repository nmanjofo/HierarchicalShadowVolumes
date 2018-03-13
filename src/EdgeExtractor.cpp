#include "EdgeExtractor.hpp"
#include "OctreeVisitor.hpp"

void EdgeExtractor::extractEdgesFromTriangles(const std::vector<Triangle>& triangles, EDGE_CONTAINER_TYPE& edges) const
{
	std::map<Edge, std::vector<glm::vec4>> tmpMap;
	
	for (const auto t : triangles)
	{
		auto e1 = Edge(t.v1, t.v2);
		tmpMap[e1].push_back(t.v3);

		auto e2 = Edge(t.v2, t.v3);
		tmpMap[e2].push_back(t.v1);

		auto e3 = Edge(t.v3, t.v1);
		tmpMap[e3].push_back(t.v2);
	}

	edges.reserve(tmpMap.size());

	for(const auto edge : tmpMap)
		edges.push_back(edge);
}