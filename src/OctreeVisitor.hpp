#pragma once

#include "Octree.hpp"
#include "Edge.hpp"
#include "GeometryOperations.hpp"
#include <memory>

class OctreeVisitor
{
public:
	OctreeVisitor(std::shared_ptr<Octree> octree);

	void addEdge(const std::pair<Edge, std::vector<glm::vec4> >& edgeInfo, int edgeID);

	void processPotentialEdges();

private:
	void _storeEdgeIsAlwaysSilhouette(EdgeSilhouetness testResult, unsigned int nodeId, unsigned int edgeID);
	void _storeEdgeIsPotentiallySilhouette(unsigned int nodeID, unsigned int edgeID);
	void _unmarkEdgeAsPotentiallySilhouetteFromNodeUp(unsigned int edgeID, unsigned int nodeID);
	void _removePotentiallySilhouetteEdgeFromNode(unsigned int edgeID, unsigned int nodeID);
	
	void _propagatePotentiallySilhouettheEdgesUp();
		bool _haveAllSyblingsEdgeAsPotential(unsigned int startingNodeID, unsigned int edgeID) const;
		void _processLevel();

	std::shared_ptr<Octree> _octree;

};
