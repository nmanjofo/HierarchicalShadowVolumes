#pragma once

#include "AABB.hpp"

class VoxelizedSpace
{
public:
	VoxelizedSpace();

	void init(const AABB& bbox, unsigned int dimX, unsigned int dimY, unsigned int dimZ);

	int getNumVoxels() const;

	bool isValid() const;

	int  getVoxelLinearIndex(unsigned int x, unsigned int y, unsigned int z) const;
	int  getVoxelLinearIndexFromPointInSpace(const glm::vec3& point) const;
	void getVoxelFromLinearIndex(int index, AABB& voxel) const;
	bool getVoxelFromPointInSpace(const glm::vec3& coords, AABB& voxel) const;
	void getVoxelFromCoords(int indexX, int indexY, int indexZ, AABB& voxel) const;

private:

	int _getDimensionIndexFromDistance(float dimensionSegmentLength, float value) const;
	bool _isLinearIndexValid(int index) const;
	bool _areCoordsValid(int x, int y, int z) const;
	void _getVoxelCoordsFromLinearIndex(int linearIndex, int& x, int& y, int& z) const;
	unsigned int _getVoxelLinearIndexFromVoxelCoords(unsigned int x, unsigned int y, unsigned int z) const;
	void _getVoxelCoordsFromPointInSpace(const glm::vec3& point, int& x, int& y, int& z) const;

	AABB  _space;

	unsigned int _numVoxelsX;
	unsigned int _numVoxelsY;
	unsigned int _numVoxelsZ;

	float _segmentLengthX;
	float _segmentLengthY;
	float _segmentLengthZ;
};
