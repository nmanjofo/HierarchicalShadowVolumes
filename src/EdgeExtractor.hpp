#pragma once

#include "Triangle.hpp"
#include "Edge.hpp"

#include <vector>
#include <map>

class EdgeExtractor
{
public:

	void extractEdgesFromTriangles(const std::vector<Triangle>& triangles, EDGE_CONTAINER_TYPE& edges) const;
};
