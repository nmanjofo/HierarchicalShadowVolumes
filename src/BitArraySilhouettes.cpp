#include "BitArraySilhouettes.hpp"

#include "GeometryOperations.hpp"

BitArraySilhouettes::BitArraySilhouettes()
{
	clear();
}

void BitArraySilhouettes::generatePerEdgeVoxelInfo(const VoxelizedSpace& lightSpace, const EDGE_CONTAINER_TYPE& edges)
{
	_edgeBitmasks.clear();

	for (const auto& edge : edges)
	{
		assert(edge.second.size() <= 2);

		//assert(edge.second.size() != 0);
		if (edge.second.size() == 0)
			continue;

		MultiBitArray ma(3, lightSpace.getNumVoxels());

		if (edge.second.size() == 1)
		{
			ma.setAllCells(int(EdgeSilhouetness::EDGE_POTENTIALLY_SILHOUETTE)); //To force multiplicity calculation due to sides winding
		}
		else
		{
			Plane p1, p2;

			GeometryOps::buildEdgeTrianglePlane(edge.first, edge.second[0], p1);
			GeometryOps::buildEdgeTrianglePlane(edge.first, edge.second[1], p2);

			const unsigned int numVoxels = lightSpace.getNumVoxels();
			for (unsigned int i = 0; i < numVoxels; ++i)
			{
				AABB voxel;
				lightSpace.getVoxelFromLinearIndex(i, voxel);

				int result = int(GeometryOps::testEdgeSpaceAabb(p1, p2, edge, voxel));

				ma.setCellContent(i, result);
			}
		}

		_edgeBitmasks.push_back(ma);
	}
}

void BitArraySilhouettes::clear()
{
	_edgeBitmasks.clear();
}
