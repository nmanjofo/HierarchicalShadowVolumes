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

size_t OctreeSilhouettes::getAccelerationStructureSizeBytes() const
{
	return _octree->getOctreeSizeBytes();
}

void OctreeSilhouettes::getSilhouetteEdgesForLightPos(const glm::vec3& lightPos, std::vector<int>& potentialEdgeIndices, std::vector<int>& silhouetteEdgeIndices)
{
	const int lowestNode = _visitor->getLowestNodeIndexFromPoint(lightPos);

	if (lowestNode < 0)
		return;

	_visitor->getSilhouttePotentialEdgesFromNodeUp(potentialEdgeIndices, silhouetteEdgeIndices, lowestNode);
}

void OctreeSilhouettes::clear()
{
	
}


