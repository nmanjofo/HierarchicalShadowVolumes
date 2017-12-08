#pragma once

#include <memory>
#include "AABB.hpp"
#include "CommonTypes.hpp"
#include <map>
#include <vector>

#define OCTREE_NUM_CHILDREN 8
/*
struct Node
{
	enum class Axis
	{
		X,
		Y,
		Z
	};
	
	Node();
	Node(const AABB& volume);
	Node(const glm::vec3& minPoint, const glm::vec3 maxPoint);

	AABB nodeVolume;

	void subdivide_generateChildren();

	int testPoint(const glm::vec3& point) const;

	Plane getSplitPlane(Axis a);

	int getRecursionLevel() const;

	std::shared_ptr<Node> children[OCTREE_NUM_CHILDREN];
	
	int id;
};

*/

class Octree
{
public:

	enum class Axis
	{
		X,
		Y,
		Z
	};

	Octree(unsigned int maxRecursionDepth);

	AABB getCell(unsigned int index);

	int getNodeParent(unsigned int nodeID) const;
	int getNodeRecursionLevel(unsigned int nodeID) const;

	int getLowestLevelCellIndexFromPointInSpace(const glm::vec3& point);

	void getEdgeIndicesFromPointInSpace(const glm::vec3& lightPos, std::vector<unsigned int>& edges);

	void splitNode(unsigned int nodeID);

	void addEdgeToNode(unsigned int edge, unsigned int nodeID);

private:
	unsigned int _maxRecursionLevel;

	std::map<unsigned int, AABB> _nodes;
	std::map<unsigned int, std::vector<unsigned int> > _edgesInNode;

	std::vector<unsigned int> _levelLimits;
};