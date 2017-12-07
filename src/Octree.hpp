#pragma once

#include <memory>
#include "AABB.hpp"

#define OCTREE_NUM_CHILDREN 8

struct Node
{
	Node();
	Node(const AABB& volume);
	Node(const glm::vec3& minPoint, const glm::vec3 maxPoint);

	AABB nodeVolume;

	void subdivide_generateChildren();

	std::shared_ptr<Node> children[OCTREE_NUM_CHILDREN];
};



class Octree
{
public:


private:
	unsigned int _recursionLevel;
};