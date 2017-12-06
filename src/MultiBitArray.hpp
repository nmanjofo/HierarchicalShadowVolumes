#pragma once

#include <vector>
#include <cstdint>

//This is limit because bitwise ops are not defined over 64-bit ints
#define MBA_ARRAY_ITEM_SIZE 32u
#define MBA_MAX_BITS_PER_CELL 32u

class MultiBitArray
{
public:
	MultiBitArray(unsigned int numBitsPerCell, unsigned int numCells);

	void resizeArrayKeepContent(unsigned int newNumCells);

	uint32_t getCellContent(unsigned int index) const;
	void setCellContent(unsigned int cellIndex, uint32_t value);

	void setAllCells(uint32_t value);

	unsigned int getNumBitsPerCell() const;
	unsigned int getNumCells() const;

	void free();
	
private:
	unsigned int _getCellArrayStartingIndex(unsigned int cellIndex) const;
	unsigned int _getCellStartPositionWithinArrayItem(unsigned int cellIndex) const;
	uint32_t _genMaskFirstNBits(unsigned int n) const;
	bool _doesCellOverflowToNextArrayItem(unsigned int startBit) const;
	unsigned int _getBitsFromArrayItem(unsigned int numBits, unsigned int startPosition, unsigned int arrayIndex) const;
	void _setBitsInArrayItem(unsigned int numBits, unsigned int startPosition, unsigned int arrayIndex, uint32_t dataContainingnNumBits);
	void _getCellAttributes(unsigned int cellIndex, unsigned int& startBit, unsigned int& startArrayIndex, unsigned int& isOverflowing) const;
	
	std::vector<uint32_t>  _array;

	unsigned int _numBitsPerCell;
	unsigned int _numCells;
};