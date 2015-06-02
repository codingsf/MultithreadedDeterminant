#pragma once
#include <memory>

class DynamicBitset
{
public:
	typedef unsigned long long BlockType;
	static const std::size_t BlockSize;
	class Bit
	{
		friend class DynamicBitset;
	public:
		operator bool() const { return (m_Block & m_Mask) != 0; }
		bool operator~() const { return (m_Block & m_Mask) == 0; }
		Bit& flip() { Flip(); return *this; }
		Bit& operator=(bool x) { Assign(x); return *this; } // for b[i] = x
		Bit& operator=(const Bit& rhs) { Assign(rhs); return *this; } // for b[i] = b[j]
	private:
		BlockType& m_Block;
		const BlockType m_Mask;

		Bit(BlockType& block, BlockType mask)
			: m_Block(block)
			, m_Mask(mask)
		{
		}
		void Set() { m_Block |= m_Mask; }
		void Reset() { m_Block &= ~m_Mask; }
		void Flip() { m_Block ^= m_Mask; }
		void Assign(bool x) { x ? Set() : Reset(); }
	};
	DynamicBitset(std::size_t size)
		: m_Size(size)
		, m_Container(new BlockType[size / BlockSize + 1])
	{
	}

	void SetAll(bool value)
	{
		int blockCount = m_Size / BlockSize + 1;
		for (int i = 0; i < blockCount; ++i)
		{
			m_Container[i] = value;
		}
	}

	Bit operator[](std::size_t index)
	{
		if (index >= m_Size)
		{
			throw std::exception("Attempt to read past dynamic bitset size!");
		}
		std::size_t blockIndex = index / BlockSize;
		BlockType mask = 1 << (BlockSize - (index % BlockSize) - 1);
		return Bit(m_Container[blockIndex], mask);
	}
private:
	std::size_t m_Size;
	std::unique_ptr<BlockType[]> m_Container;
};

