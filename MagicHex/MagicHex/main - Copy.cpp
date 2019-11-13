#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <array>
#include <string>

using std::cout;
using std::endl;
using std::stringstream;
using std::string;
using std::array;

// Value of max hex
const unsigned MaxHex = 19;
const unsigned RingSize = 12;

using RingArr = array<unsigned, RingSize>;

struct Ring
{
	// Initial is set to lowest first two valid sequences:
	// 1+18+19 = 38; 19+2+17=38
	Ring() : ring{ 1, 18, 19, 2, 17, 3, 4, 5, 6, 7, 8, 9 }
	{
	}

	string toString()
	{
		stringstream ss;
		for (auto e : ring)
		{
			ss << std::setw(2) << e << " ";
		}
		return ss.str();
	}

	void printRing()
	{
		cout << toString() << endl;
	}

	inline unsigned sum(size_t idx) { return ring[idx] + ring[idx + 1] + ring[(idx + 2) % RingSize]; };

	bool correct()
	{
		if (sum(0) != 38 || sum(2) != 38 || sum(4) != 38
			|| sum(6) != 38 || sum(8) != 38 || sum(10) != 38)
			return false;
		else
			return true;
	}

	// Returns the first index of the edge containing idx
	// In case of ties, return most counter-clockwise edge
	size_t getIdxEdgeIdx( size_t idx)
	{
		if (idx == 0)
		{
			return 0;
		}
		else if (idx % 2 == 0)
		{
			return idx - 2;
		}
		else
		{
			return idx - (idx % 2);
		}
	}

	bool plausible( int idx)
	{
		size_t idxEdge = getIdxEdgeIdx(idx);
		if (idxEdge == 0)
			return true;
		idxEdge -= 2;
		for ( size_t i = 0; i <= idxEdge; i += 2)
		{
			if (sum(i) != 38)
				return false;
		}
		return true;
	}

	bool validSide(int i)
	{
		auto s = i % 3;
		if (ring[s] + ring[s + 1] + ring[(s + 2) % RingSize] != 38)
			return false;
		else
			return true;
	}

	unsigned getSmallest(int b, int e)
	{
		unsigned smallest = 1;
		auto candidates = std::array<unsigned, MaxHex>();
		for (int i = b; i <= e; ++i)
		{
			candidates[ring[i] - 1] = 1;
		}
		for (size_t i = 0; i < MaxHex; ++i)
		{
			if (candidates[i] == 0)
				return i + 1;
		}
		return smallest;
	}

	void adjust(int idx)
	{
		for (int i = idx; i < RingSize; ++i)
		{
			auto smallest = getSmallest(0, i - 1);
			ring[i] = smallest;
		}
	}

	bool used(unsigned number, int endIdx)
	{
		for (int i = 0; i <= endIdx; ++i)
		{
			if (ring[i] == number)
				return true;
		}
		return false;
	}

	int getNextNumber(int current, size_t endIdx)
	{
		int nextNumber = current + 1;
		while (nextNumber <= MaxHex)
		{
			if (!used(nextNumber, endIdx))
				return nextNumber;
			nextNumber += 1;
		}
		return -1;
	}

	int findFirstValidEdgeIdx()
	{
		for ( int i = RingSize - 2; i >= 0; i -= 2)
		{
			if ( sum(i) == 38)
				return i;
		}
		return -1;
	}

	bool next(int idx)
	{
		while (true)
		{
			if (!plausible(idx))
			{
				--idx;
				if (idx < 0)
					return false;
				continue;
			}
			if (idx == 0)
			{
				printRing();
			}
			auto currentNumber = ring[idx];
			auto nextNumber = getNextNumber( currentNumber, idx - 1);
			if (nextNumber != -1)
			{
				ring[idx] = nextNumber;
				adjust(idx + 1);
				return true;
			}
			else
			{
				if (idx == 0)
					break;
				return next(idx - 1);
			}
		}
		return false;
	}

	bool next()
	{
		return next(RingSize - 1);
	}

	RingArr ring;
};

void main()
{
	Ring ring;

	int Limit = 2000000000;
	int count = 0;
	while (true)
	{
		if (count % 10000 == 0)
		{
			cout << std::setw(10) << count << ": ";
			ring.printRing();
		}
		if (ring.correct())
		{
			// correct() just checks outer ring
			// Try to build an inner ring
			/*
			if ( !ring.innerCorrect())
			{
				continue;
			}
			*/
			cout << "Possible solution: " << ring.toString() << endl;
		}
		if (!ring.next())
			break;
		if (++count > Limit)
			break;
	}
	ring.printRing();
}
