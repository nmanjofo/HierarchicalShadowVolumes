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

	GeometryOps::buildEdgeTrianglePlane(edgeInfo.first, edgeInfo.second[0], p1);
	GeometryOps::buildEdgeTrianglePlane(edgeInfo.first, edgeInfo.second[1], p2);

	std::stack<unsigned int> nodeStack;
	nodeStack.push(0);

	while(!nodeStack.empty())
	{
		int node = nodeStack.top();
		nodeStack.pop();

		EdgeSilhouetness testResult = GeometryOps::testEdgeSpaceAabb(p1, p2, edgeInfo, _octree->getNodeVolume(0));

		if (EDGE_IS_SILHOUETTE(testResult))
		{
			_storeEdgeIsAlwaysSilhouette(testResult, node, edgeID);

			//int parent = _octree->getNodeParent(node);
			//if (parent >= 0)
			//	_removePotentiallySilhouetteEdgeFromNode(edgeID, parent);
		}
		else if(testResult==EdgeSilhouetness::EDGE_POTENTIALLY_SILHOUETTE)
		{
			_storeEdgeIsPotentiallySilhouette(node, edgeID);
		}

		if(EDGE_IS_SILHOUETTE(testResult) || testResult == EdgeSilhouetness::EDGE_POTENTIALLY_SILHOUETTE)
		{
			const int childrenStart = _octree->getChildrenStartingId(node);

			if (childrenStart >= 0)
			{
				for (int i = 0; i < OCTREE_NUM_CHILDREN; ++i)
					nodeStack.push(childrenStart + i);
			}
		}
	}
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

void OctreeVisitor::_unmarkEdgeAsPotentiallySilhouetteFromNodeUp(unsigned int edgeID, unsigned int nodeID)
{
	int currentNode = nodeID;

	do
	{
		_removePotentiallySilhouetteEdgeFromNode(edgeID, currentNode);
		currentNode = _octree->getNodeParent(currentNode);

	} while (nodeID >= 0);
}

void OctreeVisitor::_removePotentiallySilhouetteEdgeFromNode(unsigned int edgeID, unsigned int nodeID)
{
	auto node = _octree->getNode(nodeID);

	assert(node != nullptr);

	node->edgesMayCast.erase(node->edgesMayCast.find(edgeID));
}

void OctreeVisitor::_storeEdgeIsPotentiallySilhouette(unsigned int nodeID, unsigned int edgeID)
{
	auto node = _octree->getNode(nodeID);

	assert(node != nullptr);

	node->edgesMayCast.insert(edgeID);
}

void OctreeVisitor::_postprocessEdges()
{
	const auto maxDepth = _octree->getMaxRecursionLevel();
	const unsigned int startingNode = _octree->getNumCellsInPreviousLevels(maxDepth);
	const unsigned int endNode = startingNode + ipow(OCTREE_NUM_CHILDREN, maxDepth);

	for(unsigned int i = startingNode; i<endNode; ++i)
	{
		const auto node = _octree->getNode(i);

		if(node)
		{
			
		}
	}
}
