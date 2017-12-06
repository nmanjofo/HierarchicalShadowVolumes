#include "VoxelSpace.hpp"

VoxelizedSpace::VoxelizedSpace()
{
	_numVoxelsX = _numVoxelsY = _numVoxelsZ = 0;
	_segmentLengthX = _segmentLengthY = _segmentLengthZ = 0;
}

void VoxelizedSpace::init(const AABB& bbox, unsigned int dimX, unsigned int dimY, unsigned int dimZ)
{
	_space = bbox;

	_numVoxelsX = dimX;
	_numVoxelsY = dimY;
	_numVoxelsZ = dimZ;

	if (isValid())
	{
		const glm::vec3 minPoint = _space.getMinPoint();
		const glm::vec3 maxPoint = _space.getMaxPoint();

		_segmentLengthX = (maxPoint.x - minPoint.x) / float(_numVoxelsX);
		_segmentLengthY = (maxPoint.y - minPoint.y) / float(_numVoxelsY);
		_segmentLengthZ = (maxPoint.z - minPoint.z) / float(_numVoxelsZ);
	}
	else
	{
		_segmentLengthX = _segmentLengthY = _segmentLengthZ = 0;
	}
}

int VoxelizedSpace::getNumVoxels() const
{
	return _numVoxelsX * _numVoxelsY * _numVoxelsZ;
}

bool VoxelizedSpace::isValid() const
{
	return getNumVoxels() != 0;
}

int VoxelizedSpace::getVoxelLinearIndex(unsigned int x, unsigned int y, unsigned int z) const
{
	if (x >= _numVoxelsX || y >= _numVoxelsY || z >= _numVoxelsZ)
		return -1;

	return x + _numVoxelsY * (y + _numVoxelsZ * z);
}

void VoxelizedSpace::getVoxelFromLinearIndex(int index, AABB& voxel) const
{
	if (!_isLinearIndexValid(index))
	{
		voxel = AABB::getInvalidAABB();
		return;
	}

	int x, y, z;
	_getVoxelCoordsFromLinearIndex(index, x, y, z);

	getVoxelFromCoords(x, y, z, voxel);
}

bool VoxelizedSpace::getVoxelFromPointInSpace(const glm::vec3& coords, AABB& voxel) const
{
	const glm::vec3 minPoint = _space.getMinPoint();
	const glm::vec3 maxPoint = _space.getMaxPoint();

	int dimX, dimY, dimZ;
	_getVoxelCoordsFromPointInSpace(coords, dimX, dimY, dimZ);

	getVoxelFromCoords(dimX, dimY, dimZ, voxel);

	return true;
}

int VoxelizedSpace::getVoxelLinearIndexFromPointInSpace(const glm::vec3& point) const
{
	int dimX, dimY, dimZ;
	_getVoxelCoordsFromPointInSpace(point, dimX, dimY, dimZ);

	if (!_areCoordsValid(dimX, dimY, dimZ))
		return -1;

	return _getVoxelLinearIndexFromVoxelCoords(dimX, dimY, dimZ);
}

void VoxelizedSpace::_getVoxelCoordsFromPointInSpace(const glm::vec3& point, int& x, int& y, int& z) const
{
	x = _getDimensionIndexFromDistance(_segmentLengthX, point.x - _space.getMinPoint().x);
	y = _getDimensionIndexFromDistance(_segmentLengthY, point.y - _space.getMinPoint().y);
	z = _getDimensionIndexFromDistance(_segmentLengthZ, point.z - _space.getMinPoint().z);
}

void VoxelizedSpace::getVoxelFromCoords(int indexX, int indexY, int indexZ, AABB& voxel) const
{
	if (!_areCoordsValid(indexX, indexY, indexZ))
	{
		voxel = AABB::getInvalidAABB();
		return;
	}

	glm::vec3 newMinPoint;
	glm::vec3 newMaxPoint;

	const glm::vec3 minPoint = _space.getMinPoint();
	const glm::vec3 maxPoint = _space.getMaxPoint();

	newMinPoint.x = minPoint.x + _segmentLengthX*float(indexX);
	newMinPoint.y = minPoint.y + _segmentLengthY*float(indexY);
	newMinPoint.z = minPoint.z + _segmentLengthZ*float(indexZ);
	   
	newMaxPoint.x = newMinPoint.x + _segmentLengthX;
	newMaxPoint.y = newMinPoint.y + _segmentLengthY;
	newMaxPoint.z = newMinPoint.z + _segmentLengthZ;

	voxel.setMinMaxPoints(newMinPoint, newMaxPoint);
}


int VoxelizedSpace::_getDimensionIndexFromDistance(float dimensionSegmentLength, float value) const
{
	return int(floorf(value / dimensionSegmentLength));
}

bool VoxelizedSpace::_isLinearIndexValid(int index) const
{
	return (index >= 0) && (index <int(_numVoxelsX * _numVoxelsY * _numVoxelsZ));
}

bool VoxelizedSpace::_areCoordsValid(int x, int y, int z) const
{
	if ((x > int(_numVoxelsX) || x < 0) || (y > int(_numVoxelsY) || y < 0) || (z > int(_numVoxelsZ) || z < 0))
		return false;

	return true;
}

void VoxelizedSpace::_getVoxelCoordsFromLinearIndex(int linearIndex, int& x, int& y, int& z) const
{
	if (!_isLinearIndexValid(linearIndex))
	{
		x = y = z = -1;
		return;
	}

	x = linearIndex % _numVoxelsX;
	y = (linearIndex / _numVoxelsX) % _numVoxelsY;
	z = linearIndex / (_numVoxelsX * _numVoxelsY);
}

unsigned int VoxelizedSpace::_getVoxelLinearIndexFromVoxelCoords(unsigned int x, unsigned int y, unsigned int z) const
{
	return z*(_numVoxelsX * _numVoxelsY) + y*_numVoxelsX + x;
}
