#pragma once

#include <vector>

#include "AbstractSilhouetteMethod.hpp"
#include "MultiBitArray.hpp"
#include "VoxelSpace.hpp"
#include "Edge.hpp"

struct VoxelParams
{
	unsigned int numVoxelsX, numVoxelsY, numVoxelsZ;
};


class BitArrayVoxelSilhouettes : public AbstractSilhouetteMethod
{
public:
	BitArrayVoxelSilhouettes();
	~BitArrayVoxelSilhouettes();

	void initialize(const EDGE_CONTAINER_TYPE& edges, const AABB& lightSpace, void* customParams) override;

	void clear() override;

	void getSilhouetteEdgesForLightPos(const glm::vec3& lightPos, std::vector<int>& potentialEdgeIndices, std::vector<int>& silhouetteEdgeIndices) override;

	size_t getAccelerationStructureSizeBytes() const override;

	std::vector<MultiBitArray>* getEdgeBitArrays()
	{
		return &_edgeBitmasks;
	}

private:

	void _initVoxelization();
	int  _getVoxelIndexAABBFromPos(const glm::vec3& lightPos, AABB& bbox) const;

	std::vector<MultiBitArray>	_edgeBitmasks;
	VoxelizedSpace				_voxelizedSpace;
};
