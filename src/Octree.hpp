#pragma once

#include "AABB.hpp"
#include "Edge.hpp"
#include <map>
#include <vector>
#include <set>

#define OCTREE_NUM_CHILDREN 8

int ipow(int base, int exp);

struct Node
{
	AABB volume;

	std::set<int> edgesAlwaysCast; //edge sign = winding
	std::set<unsigned int> edgesMayCast;
};


class Octree
{
public:

	Octree(unsigned int maxRecursionDepth, const AABB& volume);

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
	void deleteNodeSubtree(unsigned int nodeID);

	Node* getNode(unsigned int nodeID);
	bool nodeExists(unsigned int nodeID) const;

	unsigned int getNumCellsInPreviousLevels(int level) const;
	unsigned int getDeepestLevel() const;
	unsigned int getTotalNumNodes() const;

private:
	unsigned int _deepestLevel;

	void _generateLevelSizes();
	void _init(const AABB& volume);

	void _createChild(const AABB& parentSpace, unsigned int childID, unsigned int indexWithinParent);
	int _getCorrespondingChildIndexFromPoint(unsigned int nodeID, const glm::vec3& point) const;
	bool _isPointInsideOctree(const glm::vec3& point) const;

	std::map<unsigned int, Node> _nodes;
	std::map<unsigned int, std::vector<unsigned int> > _edgesInNode;

	std::vector<unsigned int> _levelSizesInclusiveSum;
};