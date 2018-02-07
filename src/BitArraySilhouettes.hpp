#pragma once

#include <vector>

#include "MultiBitArray.hpp"
#include "VoxelSpace.hpp"
#include "Edge.hpp"

class BitArraySilhouettes
{
public:
	BitArraySilhouettes();

	void generatePerEdgeVoxelInfo(const VoxelizedSpace& lightSpace, const EDGE_CONTAINER_TYPE& edges);

	std::vector<MultiBitArray>* getEdgeBitArrays()
	{
		return &_edgeBitmasks;
	}

	void clear();

private:
	std::vector<MultiBitArray> _edgeBitmasks;
};
