#include "BitArrayVoxelSilhouettes.hpp"

#include "GeometryOperations.hpp"
#include "VoxelSpace.hpp"
#include <iostream>

BitArrayVoxelSilhouettes::BitArrayVoxelSilhouettes() : AbstractSilhouetteMethod()
{
	BitArrayVoxelSilhouettes::clear();
}

BitArrayVoxelSilhouettes::~BitArrayVoxelSilhouettes()
{
	
}

void BitArrayVoxelSilhouettes::initialize(const EDGE_CONTAINER_TYPE& edges, const AABB& lightSpace, void* customParams)
{
	_edgeBitmasks.clear();

	const auto params = reinterpret_cast<VoxelParams*>(customParams);

	_voxelizedSpace.init(lightSpace, params->numVoxelsX, params->numVoxelsY, params->numVoxelsZ);

	unsigned int edgeIndex = 0;
	for (const auto& edge : edges)
	{
		assert(edge.second.size() <= 2);

		//assert(edge.second.size() != 0);
		if (edge.second.size() == 0)
		{
			edgeIndex++;
			continue;
		}
		MultiBitArray ma(3, _voxelizedSpace.getNumVoxels());

		if (edge.second.size() == 1)
		{
			ma.setAllCells(int(EdgeSilhouetness::EDGE_POTENTIALLY_SILHOUETTE)); //To force multiplicity calculation due to sides winding
		}
		else
		{
			Plane p1, p2;

			GeometryOps::buildEdgeTrianglePlane(edge.first, edge.second[0], p1);
			GeometryOps::buildEdgeTrianglePlane(edge.first, edge.second[1], p2);

			const unsigned int numVoxels = _voxelizedSpace.getNumVoxels();
			for (unsigned int i = 0; i < numVoxels; ++i)
			{
				AABB voxel;
				_voxelizedSpace.getVoxelFromLinearIndex(i, voxel);

				int result = int(GeometryOps::testEdgeSpaceAabb(p1, p2, edge, voxel));

				ma.setCellContent(i, result);
			}
		}

		edgeIndex++;

		_edgeBitmasks.push_back(ma);
	}
}

void BitArrayVoxelSilhouettes::clear()
{
	_edgeBitmasks.clear();
}

void BitArrayVoxelSilhouettes::getSilhouetteEdgesForLightPos(const glm::vec3& lightPos, std::vector<int>& potentialEdgeIndices, std::vector<int>& silhouetteEdgeIndices)
{
	AABB voxel;
	const int voxelIndex = _getVoxelIndexAABBFromPos(lightPos, voxel);
	
	std::cout << "Voxel index: " << voxelIndex << "\n";
	auto minP = voxel.getMinPoint();
	auto maxP = voxel.getMaxPoint();
	std::cout << "Voxel space " << minP.x << ", " << minP.y << ", " << minP.z << " Max: " << maxP.x << ", " << maxP.y << ", " << maxP.z << "\n";
	minP = voxel.getCenterPoint();
	voxel.getExtents(maxP.x, maxP.y, maxP.z);
	std::cout << "Center " << minP.x << ", " << minP.y << ", " << minP.z << " Extents: " << maxP.x << ", " << maxP.y << ", " << maxP.z << "\n";

	if (voxelIndex < 0)
		return;

	int edgeIndex = 0;
	for(const auto _edgeEntry : _edgeBitmasks)
	{
		int result = _edgeEntry.getCellContent(voxelIndex);

		if (EDGE_IS_SILHOUETTE(result))
		{
			const int multiplicitySign = (result == int(EdgeSilhouetness::EDGE_IS_SILHOUETTE_PLUS)) + (-1)*(result == int(EdgeSilhouetness::EDGE_IS_SILHOUETTE_MINUS));
			silhouetteEdgeIndices.push_back(multiplicitySign * edgeIndex);
		}

		if (result == int(EdgeSilhouetness::EDGE_POTENTIALLY_SILHOUETTE))
			potentialEdgeIndices.push_back(edgeIndex);

		++edgeIndex;
	}
}

int BitArrayVoxelSilhouettes::_getVoxelIndexAABBFromPos(const glm::vec3& lightPos, AABB& bbox) const
{
	const int voxelIndex = _voxelizedSpace.getVoxelLinearIndexFromPointInSpace(lightPos);

	if (voxelIndex < 0)
	{
		std::cerr << "Invalid light position with respect to voxelized space\n";
		bbox = AABB();

		return -1;
	}

	_voxelizedSpace.getVoxelFromLinearIndex(voxelIndex, bbox);
	
	return voxelIndex;
}

uint64_t BitArrayVoxelSilhouettes::getAccelerationStructureSizeBytes() const
{
	return _edgeBitmasks.size() * _edgeBitmasks[0].getNumCells() * (MBA_MAX_BITS_PER_CELL / 8);
}
