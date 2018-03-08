#include "OctreeSilhouettes.hpp"
#include "Application.hpp"

void OctreeSilhouettes::initialize(const EDGE_CONTAINER_TYPE& edges, const AABB& lightSpace, void* customParams)
{
	const auto params = reinterpret_cast<OctreeParams*>(customParams);

	_octree = std::make_shared<Octree>(params->maxDepthLevel, lightSpace);
	_visitor = std::make_shared<OctreeVisitor>(_octree);

	_loadOctreeBottomTop(edges);
}

void OctreeSilhouettes::_loadOctreeBottomTop(const EDGE_CONTAINER_TYPE& edges)
{
	_visitor->addEdges(edges);
}

void OctreeSilhouettes::_loadOctreeTopBottom(const EDGE_CONTAINER_TYPE& edges)
{
	unsigned int i = 0;
	for (const auto edge : edges)
	{
		_visitor->addEdge(edge, i);
		++i;
	}
}

uint64_t OctreeSilhouettes::getAccelerationStructureSizeBytes() const
{
	return _octree->getOctreeSizeBytes();
}

void OctreeSilhouettes::getSilhouetteEdgesForLightPos(const glm::vec3& lightPos, std::vector<int>& potentialEdgeIndices, std::vector<int>& silhouetteEdgeIndices)
{
	const int lowestNode = _visitor->getLowestNodeIndexFromPoint(lightPos);

	std::cout << "Light containing node: " << lowestNode << std::endl;

	if (lowestNode < 0)
		return;

	const auto n = _octree->getNode(lowestNode);
	auto minP = n->volume.getMinPoint();
	auto maxP = n->volume.getMaxPoint();
	std::cout << "Node space " << minP.x << ", " << minP.y << ", " << minP.z << " Max: " << maxP.x << ", " << maxP.y << ", " << maxP.z << "\n";
	minP = n->volume.getCenterPoint();
	n->volume.getExtents(maxP.x, maxP.y, maxP.z);
	std::cout << "Center " << minP.x << ", " << minP.y << ", " << minP.z << " Extents: " << maxP.x << ", " << maxP.y << ", " << maxP.z << "\n";

	if (lowestNode < 0)
		return;

	_visitor->getSilhouttePotentialEdgesFromNodeUp(potentialEdgeIndices, silhouetteEdgeIndices, lowestNode);
}

void OctreeSilhouettes::clear()
{
	
}

void OctreeSilhouettes::printLevelOccupancies() const
{
	for(int i = _octree->getDeepestLevel(); i > 0; --i)
	{
		const auto firstNode = _octree->getLevelFirstNodeID(i);
		const auto levelSize = _octree->getNumNodesInLevel(i);

		uint64_t numPotential = 0;
		uint64_t numSilhouette = 0;
		for(int n = firstNode; n<(firstNode + levelSize); ++n)
		{
			const auto node = _octree->getNode(n);

			if(n)
			{
				numPotential += node->edgesMayCast.size();
				numSilhouette += node->edgesAlwaysCast.size();
			}
		}

		std::cout << "Level " << i << ": potential " << numPotential << " silhouette " << numSilhouette << std::endl;
	}
}