#pragma once 

#include "AbstractSilhouetteMethod.hpp"
#include "OctreeVisitor.hpp"
#include "Octree.hpp"

struct OctreeParams
{
	unsigned int maxDepthLevel;
};


class OctreeSilhouettes : public AbstractSilhouetteMethod
{
public:
	void getSilhouetteEdgesForLightPos(const glm::vec3& lightPos, std::vector<int>& potentialEdgeIndices, std::vector<int>& silhouetteEdgeIndices) override;

	void initialize(const EDGE_CONTAINER_TYPE& edges, const AABB& lightSpace, void* customParams) override;

	void clear() override;

	size_t getAccelerationStructureSizeBytes() const override;

private:

	void _loadOctreeTopBottom(const EDGE_CONTAINER_TYPE& edges);
	void _loadOctreeBottomTop(const EDGE_CONTAINER_TYPE& edges);

	std::shared_ptr<Octree>			_octree;
	std::shared_ptr<OctreeVisitor>	_visitor;
};
