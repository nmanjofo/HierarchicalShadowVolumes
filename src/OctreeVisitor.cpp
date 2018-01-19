#include "OctreeVisitor.hpp"
#include "Plane.hpp"
#include "GeometryOperations.hpp"

#include <stack>

OctreeVisitor::OctreeVisitor(std::shared_ptr<Octree> octree)
{
	_octree = octree;
}

void OctreeVisitor::addEdge(const std::pair<Edge, std::vector<glm::vec4> >& edgeInfo, int edgeID)
{
	Plane p1, p2;

	const auto sz = edgeInfo.second.size();

	assert(sz <= 2);

	if (sz == 0)
		return;
	if(sz==1)
	{
		_storeEdgeIsPotentiallySilhouette(0, edgeID);
		return;
	}

	GeometryOps::buildEdgeTrianglePlane(edgeInfo.first, edgeInfo.second[0], p1);
	GeometryOps::buildEdgeTrianglePlane(edgeInfo.first, edgeInfo.second[1], p2);

	std::stack<unsigned int> nodeStack;
	nodeStack.push(0);

	while(!nodeStack.empty())
	{
		int node = nodeStack.top();
		nodeStack.pop();

		if (!_octree->nodeExists(node))
			_octree->splitNode(_octree->getNodeParent(node));

		EdgeSilhouetness testResult = GeometryOps::testEdgeSpaceAabb(p1, p2, edgeInfo, _octree->getNodeVolume(node));

		if (EDGE_IS_SILHOUETTE(testResult))
			_storeEdgeIsAlwaysSilhouette(testResult, node, edgeID);
		else if (testResult == EdgeSilhouetness::EDGE_POTENTIALLY_SILHOUETTE)
		{
			const int childrenStart = _octree->getChildrenStartingId(node);

			if (childrenStart >= 0)
			{
				for (int i = 0; i < OCTREE_NUM_CHILDREN; ++i)
					nodeStack.push(childrenStart + i);
			}
			else
				_storeEdgeIsPotentiallySilhouette(node, edgeID);
		}
	}
}

void OctreeVisitor::addEdges(const std::map<Edge, std::vector<glm::vec4> >& edges)
{
	//_expandWholeOctree();

	//_addEdgesOnLowestLevel(edges);
}

void _addEdgesOnLowestLevel(const std::map<Edge, std::vector<glm::vec4> >& _edges)
{
	
}

void _addEdgesSyblings(const std::map<Edge, std::vector<glm::vec4> >& _edges, unsigned int startingID)
{
	
}

void OctreeVisitor::_storeEdgeIsAlwaysSilhouette(EdgeSilhouetness testResult, unsigned int nodeId, unsigned int edgeID)
{
	auto node = _octree->getNode(nodeId);

	assert(node != nullptr);

	if (testResult == EdgeSilhouetness::EDGE_IS_SILHOUETTE_PLUS)
		node->edgesAlwaysCast.insert(edgeID);
	
	if (testResult == EdgeSilhouetness::EDGE_IS_SILHOUETTE_MINUS)
		node->edgesAlwaysCast.insert(-int(edgeID));
}

void OctreeVisitor::_storeEdgeIsPotentiallySilhouette(unsigned int nodeID, unsigned int edgeID)
{
	auto node = _octree->getNode(nodeID);

	assert(node != nullptr);

	node->edgesMayCast.insert(edgeID);
}

void OctreeVisitor::processPotentialEdges()
{
	_propagatePotentiallySilhouettheEdgesUp();
}

void OctreeVisitor::_propagatePotentiallySilhouettheEdgesUp()
{
	const unsigned int maxLevel = _octree->getDeepestLevel();

	for(unsigned int i = maxLevel; i>0; --i)
		_processPotentialEdgesInLevel(i);
}


OctreeVisitor::TestResult OctreeVisitor::_haveAllSyblingsEdgeAsPotential(unsigned int startingNodeID, unsigned int edgeID) const
{	
	TestResult retval = TestResult::NON_EXISTING;
	
	for(unsigned int i=0; i<OCTREE_NUM_CHILDREN; ++i)
	{
		const auto node = _octree->getNode(startingNodeID + i);
		
		if (!node)
			continue;

		if (node->edgesMayCast.find(edgeID) == node->edgesMayCast.end())
			return TestResult::FALSE;
		
		retval = TestResult::TRUE;
	}

	return retval;
}

void OctreeVisitor::_processPotentialEdgesInLevel(unsigned int level)
{
	assert(level > 0);
	const int startingID = _getFirstNodeIdInLevel(level);
	
	assert(startingID >= 0);

	const unsigned int stopId = ipow(OCTREE_NUM_CHILDREN, level) + startingID;

	unsigned int currentID = startingID;

	while(currentID<stopId)
	{
		std::set<unsigned int> potentialEdgesSyblings;
		_getAllPotentialEdgesSyblings(currentID, potentialEdgesSyblings);

		for(auto edge : potentialEdgesSyblings)
		{
			auto result = _haveAllSyblingsEdgeAsPotential(currentID, edge);

			if (result == TestResult::TRUE)
			{
				_assignPotentialEdgeToNodeParent(currentID, edge);
				_removePotentialEdgeFromSyblings(currentID, edge);
			}
		}

		currentID += OCTREE_NUM_CHILDREN;
	}
}

int	OctreeVisitor::_getFirstNodeIdInLevel(unsigned int level) const
{
	return _octree->getNumCellsInPreviousLevels(level);
}

void OctreeVisitor::_getAllPotentialEdgesSyblings(unsigned int startingID, std::set<unsigned int>& edges) const
{
	for(unsigned int i=0; i<OCTREE_NUM_CHILDREN; ++i)
	{
		const auto node = _octree->getNode(startingID + i);

		if(node!=nullptr)
			edges.insert(node->edgesMayCast.begin(), node->edgesMayCast.end());
	}
}

void OctreeVisitor::_removePotentialEdgeFromSyblings(unsigned int startingID, unsigned int edge)
{
	for(unsigned int i=0; i<OCTREE_NUM_CHILDREN; ++i)
	{
		auto node = _octree->getNode(startingID + i);

		if(node)
			node->edgesMayCast.erase(edge);
	}
}

void OctreeVisitor::_assignPotentialEdgeToNodeParent(unsigned int node, unsigned int edge)
{
	const int parent = _octree->getNodeParent(node);

	assert(parent >= 0);

	auto n = _octree->getNode(parent);

	if (n)
		n->edgesMayCast.insert(edge);
}

void OctreeVisitor::cleanEmptyNodes()
{
	/*const int startingID = _getFirstNodeIdInLevel(_octree->getDeepestLevel());
	const int bottomLevelSize = ipow(OCTREE_NUM_CHILDREN, _octree->getDeepestLevel());

	for(int i = startingID; i<(startingID + bottomLevelSize); ++i)
	{
		auto node = _octree->getNode(i);

		if (node && !node->edgesAlwaysCast.size() && !node->edgesMayCast.size())
			_octree->deleteNodeSubtree(i);
	}*/

	for(unsigned int level = _octree->getDeepestLevel(); level>0; --level)
	{
		_processEmptyNodesInLevel(level);
	}
}

void OctreeVisitor::_processEmptyNodesInLevel(unsigned int level)
{
	assert(level > 0);
	const int startingID = _getFirstNodeIdInLevel(level);

	assert(startingID >= 0);

	const unsigned int stopId = ipow(OCTREE_NUM_CHILDREN, level) + startingID;

	unsigned int currentID = startingID;

	const unsigned int deepestLevel = _octree->getDeepestLevel();

	while (currentID < stopId)
	{
		_processEmptyNodesSyblingsParent(currentID);

		currentID += OCTREE_NUM_CHILDREN;
	}
}

void OctreeVisitor::_processEmptyNodesSyblingsParent(unsigned int first)
{
	unsigned int numRemoved = 0;
	
	for(unsigned int i=0; i<OCTREE_NUM_CHILDREN; ++i)
	{
		auto node = _octree->getNode(first + i);

		if(node && !node->edgesMayCast.size() && !node->edgesAlwaysCast.size())
		{
			_octree->deleteNode(first + i);
			++numRemoved;
		}
		else
			++numRemoved;
	}

	if(numRemoved==OCTREE_NUM_CHILDREN)
	{
		const int parentID = _octree->getNodeParent(first);
		auto parent = _octree->getNode(parentID);

		if (parent && !parent->edgesMayCast.size() && !parent->edgesAlwaysCast.size())
			_octree->deleteNode(parentID);
	}
}


