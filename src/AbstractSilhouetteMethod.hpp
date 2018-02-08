#pragma once

#include <vector>

#include "Edge.hpp"
#include "AABB.hpp"

class AbstractSilhouetteMethod
{
public:

	AbstractSilhouetteMethod() {}
	virtual ~AbstractSilhouetteMethod() {}

	//Sem dve varianty - potencialne a iste
	virtual void getSilhouetteEdgesForLightPos(const glm::vec3& lightPos, std::vector<int>& potentialEdgeIndices, std::vector<int>& silhouetteEdgeIndices) = 0;

	virtual void initialize(const EDGE_CONTAINER_TYPE& edges, const AABB& lightSpace, void* customParams) = 0;

	virtual void clear() = 0;

	virtual size_t getAccelerationStructureSizeBytes() const = 0;

protected:

};