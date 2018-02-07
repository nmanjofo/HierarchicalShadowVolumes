#pragma once

#include <vector>

#include "Edge.hpp"
#include "AABB.hpp"

class AbstractSilhouetteMethod
{
public:

	virtual ~AbstractSilhouetteMethod();

	//Sem dve varianty - potencialne a iste
	virtual void getSilhouetteEdges() = 0;

	virtual void initialize(const EDGE_CONTAINER_TYPE& edges, const AABB& lightSpace) = 0;

	virtual void clear() = 0;

protected:

};