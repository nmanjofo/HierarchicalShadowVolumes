#include "Octree.hpp"

#include "GeometryOperations.hpp"

int ipow(int base, int exp)
{
	int result = 1;
	while (exp)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}

	return result;
}

Octree::Octree(unsigned int maxRecursionDepth, const AABB& volume)
{
	_maxRecursionDepth = maxRecursionDepth;

	_generateLimits();

	_init(volume);
}

void Octree::_generateLimits()
{
	_levelSizes.clear();

	unsigned int prefixSum = 0;
	//TODO - je tu ozaj "<"?
	for(unsigned int i=0; i<=_maxRecursionDepth; ++i)
	{
		const unsigned int levelSize = ipow(8, i);
		_levelSizes.push_back(levelSize + prefixSum);
		prefixSum += levelSize;
	}
}

void Octree::_init(const AABB& volume)
{
	Node n;
	n.volume = volume;

	_nodes[0] = n;
}

Node* Octree::getNode(unsigned int nodeID)
{
	auto n = _nodes.find(nodeID);

	if (n == _nodes.end())
		return nullptr;

	return &n->second;
}

AABB Octree::getNodeVolume(unsigned int nodeID) const
{
	assert(_nodeExists(nodeID));
	
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
	for(auto size : _levelSizes)
	{
		if (nodeID < size)
			return level;

		++level;
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
	unsigned int currentNode = 0;
	
	while(recursionLevel<=_maxRecursionDepth)
	{
		const int startingChild = getChildrenStartingId(currentNode);

		if (!_nodeExists(startingChild))
			break;

		const int childIndex = _getCorrespondingChildIndexFromPoint(currentNode, point);

		currentNode = startingChild + childIndex;
		++recursionLevel;
	}

	return currentNode;
}

int Octree::_getCorrespondingChildIndexFromPoint(unsigned int nodeID, const glm::vec3& point) const
{
	const glm::vec3 centerPoint = getNodeVolume(nodeID).getCenterPoint();

	int r = (point.x >= centerPoint.x) + 2 * (point.y >= centerPoint.y) + 4 * (point.z >= centerPoint.z);
	return r;
}

bool Octree::_nodeExists(unsigned int nodeID) const
{
	return (nodeID<getNumCellsInPreviousLevels(_maxRecursionDepth)) && (_nodes.find(nodeID) != _nodes.end());
}

void Octree::splitNode(unsigned int nodeID)
{
	AABB nodeVolume = getNodeVolume(nodeID);

	const int startingIndex = getChildrenStartingId(nodeID);

	for(unsigned int i=0; i<OCTREE_NUM_CHILDREN; ++i)
		_createChild(nodeVolume, startingIndex + i, i);
}

unsigned int Octree::getNumCellsInPreviousLevels(int level) const
{
	assert(level < int(_levelSizes.size()));

	if (level < 0 || level> _maxRecursionDepth)
		return 0;

	return _levelSizes[level];
}

int Octree::getChildrenStartingId(unsigned int nodeID) const
{
	const int nodeLevel = getNodeRecursionLevel(nodeID);

	assert(nodeLevel >= 0);

	const int idInLevel = getNodeIdInLevel(nodeID, nodeLevel);

	assert(idInLevel >= 0);

	return OCTREE_NUM_CHILDREN*idInLevel + getNumCellsInPreviousLevels(nodeLevel);
}

void Octree::_createChild(const AABB& parentSpace, unsigned int newNodeId, unsigned int indexWithinParent)
{
	assert(_nodes.find(newNodeId) == _nodes.end());

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

	glm::vec3 minPointOffset = glm::vec3(isInPlusX * parentHalfExtentX, isInPlusY * parentHalfExtentY, isInPlusZ * parentHalfExtentZ);

	minPoint = minPoint + minPointOffset;
	glm::vec3 maxPoint = minPoint + glm::vec3(parentHalfExtentX, parentHalfExtentY, parentHalfExtentZ);

	n.volume = AABB(minPoint, maxPoint);

	_nodes[newNodeId] = n;
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

	return GeometryOps::testAabbPointIsInsideOrOn(iter->second.volume, point);
}

unsigned int Octree::getMaxRecursionLevel() const
{
	return _maxRecursionDepth;
}
