#include "Octree.hpp"

Octree::Octree(unsigned int maxRecursionDepth)
{
	_maxRecursionLevel = maxRecursionDepth;

	_generateLimits();
}

void Octree::_generateLimits()
{
	_numCellsInPreviousLevels.clear();

	unsigned int prefixSum = 0;
	for(unsigned int i=0; i<_maxRecursionLevel; ++i)
	{
		const unsigned int levelSize = 2 ^ i;
		_numCellsInPreviousLevels.push_back(levelSize + prefixSum);
		prefixSum += levelSize;
	}
}

AABB Octree::getNodeVolume(unsigned int nodeID) const
{
	auto iter = _nodes.find(nodeID);

	if (iter != _nodes.end())
		return iter->second.volume;

	return AABB();
}

int Octree::getNodeParent(unsigned int nodeID) const
{
	if (nodeID == 0)
		return -1;
	
	const int nodeLevel = getNodeRecursionLevel(nodeID);
	
	assert(nodeLevel >= 0);

	const int idInLevel = getNodeIdInLevel(nodeID, nodeLevel);

	const int parentRelativeID = idInLevel / OCTREE_NUM_CHILDREN;

	return parentRelativeID + getNumCellsInPreviousLevels(nodeLevel - 2);
}

int Octree::getNodeRecursionLevel(unsigned int nodeID) const
{
	int level = 0;
	for(auto size : _numCellsInPreviousLevels)
	{
		if (nodeID < size)
			return level;
	}

	return -1;
}

int Octree::getNodeIdInLevel(unsigned int nodeID) const
{
	int level = getNodeRecursionLevel(nodeID);

	return level > 0 ? getNodeIdInLevel(nodeID, level) : -1;
}

int Octree::getNodeIdInLevel(unsigned int nodeID, unsigned int level) const
{
	return nodeID - getNumCellsInPreviousLevels(level - 1);
}

int Octree::getLowestLevelCellIndexFromPointInSpace(const glm::vec3& point)
{
	if (!_isPointInsideOctree(point))
		return -1;

	unsigned int recursionLevel = 0;

	while(recursionLevel<=_maxRecursionLevel)
	{
		
	}
}

void Octree::getEdgeIndicesFromPointInSpace(const glm::vec3& lightPos, std::vector<unsigned int>& edges)
{
	
}

void Octree::splitNode(unsigned int nodeID)
{
	AABB nodeVolume = getNodeVolume(nodeID);

	const int startingIndex = getChildrenStartingId(nodeID);

	for(unsigned int i=0; i<OCTREE_NUM_CHILDREN; ++i)
		_createChild(nodeVolume, startingIndex + i, i);
}

void Octree::addEdgeToNode(unsigned int edge, unsigned int nodeID)
{
	
}

int Octree::getNumCellsInPreviousLevels(int level) const
{
	assert(level >= _numCellsInPreviousLevels.size());

	if (level < 0)
		return 0;

	return _numCellsInPreviousLevels[level];
}

int Octree::getChildrenStartingId(unsigned int nodeID) const
{
	const int nodeLevel = getNodeRecursionLevel(nodeID);

	assert(nodeLevel >= 0);

	const int idInLevel = getNodeIdInLevel(nodeID, nodeLevel);

	assert(idInLevel >= 0);

	return OCTREE_NUM_CHILDREN*idInLevel + getNumCellsInPreviousLevels(nodeLevel);
}

void Octree::_createChild(const AABB& parentSpace, unsigned int childID, unsigned int indexWithinParent)
{
	assert(_nodes.find(childID) == _nodes.end());

	Node n;
	glm::vec3 minPoint = parentSpace.getMinPoint();

	const bool isInPlusX = (indexWithinParent & 1) != 0;
	const bool isInPlusY = (indexWithinParent & 2) != 0;
	const bool isInPlusZ = (indexWithinParent & 4) != 0;

	float parentHalfExtentX, parentHalfExtentY, parentHalfExtentZ;
	parentSpace.getExtents(parentHalfExtentX, parentHalfExtentY,parentHalfExtentZ);

	parentHalfExtentX /= 2.0f;
	parentHalfExtentY /= 2.0f;
	parentHalfExtentZ /= 2.0f;

	glm::vec3 span = glm::vec3(isInPlusX * parentHalfExtentX, isInPlusY * parentHalfExtentY, isInPlusZ * parentHalfExtentZ);

	minPoint = minPoint + span;
	glm::vec3 maxPoint = minPoint + glm::vec3(parentHalfExtentX, parentHalfExtentY, parentHalfExtentZ);

	n.volume = AABB(minPoint, maxPoint);

	_nodes[childID] = n;
}

int Octree::getNodeIndexWithinParent(unsigned int nodeID) const
{
	const int parent = getNodeParent(nodeID);

	assert(parent >= 0);

	return getNodeIndexWithinParent(nodeID, parent);
}

int Octree::getNodeIndexWithinParent(unsigned int nodeID, unsigned int parent) const
{
	const int startID = getChildrenStartingId(parent);

	return nodeID - startID;
}

bool Octree::_isPointInsideOctree(const glm::vec3& point) const
{
	auto iter = _nodes.find(0);
	
	assert(iter != _nodes.end());

	return iter->second.volume.testIsPointInside(point);
}