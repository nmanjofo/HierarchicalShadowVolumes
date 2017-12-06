#pragma once

inline void SetBit(uint32_t& bitField, unsigned int bit)
{
    bitField |= 1 << bit;
}

inline void ClearBit(uint32_t& bitField, unsigned int bit)
{
    bitField &= ~(1 << bit);
}

inline bool IsBitSet(unsigned int& bitField, unsigned int bit)
{
	if ((bitField >> bit) & 1)
		return true;
	else
		return false;
}

inline void FlipBit(uint32_t& bitfield, unsigned int bit)
{
	if (IsBitSet(bitfield, bit))
		ClearBit(bitfield, bit);
	else
		SetBit(bitfield, bit);
}
