#include <cassert>

#include "MultiBitArray.hpp"
#include "BitOperations.h"


MultiBitArray::MultiBitArray(unsigned int numBitsPerCell, unsigned int numCells)
{
	if (numCells == 0)
	{
		free();

		return;
	}

	_numCells = 0;
	_numBitsPerCell = numBitsPerCell;

	if(numCells)
		resizeArrayKeepContent(numCells);
}

void MultiBitArray::free()
{
	_array.clear();

	_numBitsPerCell = _numCells = 0;
}

void MultiBitArray::resizeArrayKeepContent(unsigned int newNumCells)
{
	if (newNumCells == 0)
	{
		free();
		return;
	}

	const unsigned int oldNumCells = _numCells;

	_numCells = newNumCells;

	if (newNumCells <= oldNumCells)
		return;

	const unsigned int totalBits = newNumCells * _numBitsPerCell;
	const unsigned int arraySize = unsigned int(ceilf(float(totalBits)/ MBA_ARRAY_ITEM_SIZE));

	if (arraySize <= _array.size())
		return;

	_array.resize(arraySize, 0);
}

unsigned int MultiBitArray::getNumBitsPerCell() const
{
	return _numBitsPerCell;
}

unsigned int MultiBitArray::getNumCells() const
{
	return _numCells;
}

uint32_t MultiBitArray::getCellContent(unsigned int cellIndex) const
{
	assert(cellIndex < _numCells);
	
	uint32_t retval = 0;

	unsigned int startArrayIndex = 0, startBit = 0, isOverflowing = 0;
	_getCellAttributes(cellIndex, startBit, startArrayIndex, isOverflowing);

	const unsigned int grabSize = (1- isOverflowing)*_numBitsPerCell + isOverflowing*(MBA_ARRAY_ITEM_SIZE - startBit);

	retval = _getBitsFromArrayItem(grabSize, startBit, startArrayIndex);

	if (isOverflowing)
	{
		const unsigned int remainingSize = _numBitsPerCell - grabSize;
		const uint32_t remainingBits = _getBitsFromArrayItem(remainingSize, 0, startArrayIndex + 1);
		retval |= remainingBits << grabSize;
	}

	return retval;
}

uint32_t MultiBitArray::_genMaskFirstNBits(unsigned int n) const
{
	unsigned int mask = 0;

	for (unsigned int i = 0; i < n; ++i)
		mask |= 1 << i;

	return mask;
}

unsigned int MultiBitArray::_getCellArrayStartingIndex(unsigned int cellIndex) const
{
	assert(cellIndex <= _numCells);

	return (cellIndex * _numBitsPerCell) / MBA_ARRAY_ITEM_SIZE;
}

unsigned int MultiBitArray::_getCellStartPositionWithinArrayItem(unsigned int cellIndex) const
{
	assert(cellIndex <= _numCells);

	return (cellIndex * _numBitsPerCell) % MBA_ARRAY_ITEM_SIZE;
}

bool MultiBitArray::_doesCellOverflowToNextArrayItem(unsigned int startBit) const
{
	return (startBit + _numBitsPerCell) > MBA_ARRAY_ITEM_SIZE;
}

unsigned int MultiBitArray::_getBitsFromArrayItem(unsigned int numBits, unsigned int startBitPosition, unsigned int arrayIndex) const
{
	assert((startBitPosition + numBits) <= MBA_ARRAY_ITEM_SIZE);
	assert(arrayIndex < _array.size());

	uint32_t val = _array[arrayIndex];
	val = val >> startBitPosition;

	const uint32_t mask = _genMaskFirstNBits(numBits);

	return val & mask;
}

void MultiBitArray::_setBitsInArrayItem(unsigned int numBits, unsigned int startBitPosition, unsigned int arrayIndex, uint32_t dataContainingnNumBits)
{
	assert((startBitPosition + numBits) <= MBA_ARRAY_ITEM_SIZE);
	assert(arrayIndex < _array.size());

	for (unsigned int i = 0; i < numBits; ++i)
	{
		bool setBit = dataContainingnNumBits & 1;
		if (setBit)
			SetBit(_array[arrayIndex], startBitPosition + i);
		else
			ClearBit(_array[arrayIndex], startBitPosition + i);

		dataContainingnNumBits = dataContainingnNumBits >> 1;
	}

	//_array[arrayIndex] ^= (dataContainingnNumBits << startBitPosition);
}

void MultiBitArray::setCellContent(unsigned int cellIndex, uint32_t value)
{
	assert(cellIndex < _numCells);

	unsigned int startArrayIndex = 0, startBit = 0, isOverflowing = 0;
	_getCellAttributes(cellIndex, startBit, startArrayIndex, isOverflowing);

	const unsigned int setSize = (1 - isOverflowing)*_numBitsPerCell + isOverflowing*(MBA_ARRAY_ITEM_SIZE - startBit);
	//ORing more data in this step does not matter, because trialing data will be shifted out anyway
	_setBitsInArrayItem(setSize, startBit, startArrayIndex, value);

	if (isOverflowing)
	{
		const unsigned int remainingNumBits = _numBitsPerCell - setSize;
		value = value >> (setSize);
		_setBitsInArrayItem(remainingNumBits, 0, startArrayIndex + 1, value);
	}
}

void MultiBitArray::setAllCells(uint32_t value)
{
	for (unsigned int i = 0; i < _numCells; ++i)
		setCellContent(i, value);
}

void MultiBitArray::_getCellAttributes(unsigned int cellIndex, unsigned int& startBit, unsigned int& startArrayIndex, unsigned int& isOverflowing) const
{
	startArrayIndex = _getCellArrayStartingIndex(cellIndex);
	startBit = _getCellStartPositionWithinArrayItem(cellIndex);
	isOverflowing = _doesCellOverflowToNextArrayItem(startBit) ? 1 : 0;
}
