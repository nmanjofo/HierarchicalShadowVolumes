#pragma once

#include "AABB.hpp"
#include "CommonTypes.hpp"
#include <map>
#include <vector>
#include <set>

#define OCTREE_NUM_CHILDREN 8

struct Node
{
	AABB volume;

	std::set<unsigned int> edgesAlwaysCast;
	std::set<unsigned int> edgesMayCast;
};


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

	AABB getNodeVolume(unsigned int index) const;

	int getNodeParent(unsigned int nodeID) const;
	int getNodeRecursionLevel(unsigned int nodeID) const;
	int getNodeIdInLevel(unsigned int nodeID) const;
	int getNodeIdInLevel(unsigned int nodeID, unsigned int level) const;
	int getNodeIndexWithinParent(unsigned int nodeID) const;
	int getNodeIndexWithinParent(unsigned int nodeID, unsigned int parent) const;

	int getChildrenStartingId(unsigned int nodeID) const;

	int getLowestLevelCellIndexFromPointInSpace(const glm::vec3& point);

	void getEdgeIndicesFromPointInSpace(const glm::vec3& lightPos, std::vector<unsigned int>& edges);

	void splitNode(unsigned int nodeID);

	void addEdgeToNode(unsigned int edge, unsigned int nodeID);

	int getNumCellsInPreviousLevels(int level) const;

private:
	unsigned int _maxRecursionLevel;

	void _generateLimits();

	void _createChild(const AABB& parentSpace, unsigned int childID, unsigned int indexWithinParent);
	bool _isPointInsideOctree(const glm::vec3& point) const;

	std::map<unsigned int, Node> _nodes;
	std::map<unsigned int, std::vector<unsigned int> > _edgesInNode;

	std::vector<unsigned int> _numCellsInPreviousLevels;
};