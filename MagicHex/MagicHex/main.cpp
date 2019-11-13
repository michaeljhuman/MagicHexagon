#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
//#include <set>
#include <bitset>
#include <string>

using std::cout;
using std::endl;
using std::stringstream;
using std::string;
using std::array;

// Value of max hex
const unsigned TileCount = 19;
const unsigned RingSize = 12;
const unsigned InnerTileCount = 7;

using RingArr = array<unsigned, RingSize>;
using InnerTilesArray = array<unsigned, InnerTileCount>;

struct Ring
{
	// It would be easy to set the initial edges to sum to 38
	// But it doesn't seem worth the effort.
	Ring() : ring{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }
	{
	}

	string toString()
	{
		stringstream ss;
		for (auto e : ring)
		{
			ss << e << " ";
		}
		return ss.str();
	}

	void printRing()
	{
		cout << toString() << endl;
	}

	unsigned sum(size_t idx)
	{
		return ring[idx] + ring[idx + 1] + ring[(idx + 2) % 12];
	};

	bool isCorrect()
	{
		// Check sum of every edge
		for ( size_t edgeIdx = 0; edgeIdx < RingSize; edgeIdx += 2)
		{
			if (sum(edgeIdx) != 38)
				return false;
		}
		return true;
	}

	// Check if sum of edge for the index 'idx' is correct
	bool isCorrect( size_t idx)
	{
		if (sum(idx) == 38)
			return true;
		else
			return false;
	}

	// Check if innerTiles plus the outer ring, is a valid
	// solution.  All rows must sum to 38
	bool isCorrect( const InnerTilesArray &innerTiles)
	{
		// Need to check sums on magic hexagon.  
		// Presume magic hexagon is oriented with a top edge
		// ring[0] is leftmost of left edge
		// innerTiles are presumed to be in this order:
		// 0, 1 - "top" line
		// 2, 3, 4 - "middle" line
		// 5, 6 - "bottom" line

		// TODO: Is there a more elegant way to iterate over the lines to be
		// summed?
		if (ring[11] + innerTiles[0] + innerTiles[1] + ring[3] != 38)
			return false;
		if (ring[10] + innerTiles[2] + innerTiles[3] + innerTiles[4] + ring[4] != 38)
			return false;
		if (ring[9] + innerTiles[5] + innerTiles[6] + ring[5] != 38)
			return false;

		if (ring[1] + innerTiles[0] + innerTiles[2] + ring[9] != 38)
			return false;
		if (ring[2] + innerTiles[1] + innerTiles[3] + innerTiles[5] + ring[8] != 38)
			return false;
		if (ring[3] + innerTiles[4] + innerTiles[6] + ring[7] != 38)
			return false;

		return true;
	}

	// Returns true, if a permutation can be found from
	// the numbers in "innerTiles", such that all rows
	// in the magic hexagon sum to 38.
	// innerTiles is modified and on success, and contains
	// the inner tiles in order from top left of top row to
	// bottom right of bottom row ( see comments in isCorrect().)
	// The hexagon is oriented with top edge on top, and index 0 
	// of the outer ring is the first til of the top edge.
	bool validInnerTiles( InnerTilesArray &innerTiles)
	{
		// Populate all tiles
		size_t j = 0;
		for (int i = 1; i <= TileCount; ++i)
		{
			if (!used(i, RingSize - 1))
			{
				assert(j < 7);
				innerTiles[j++] = i;
			}
		}
		std::sort(innerTiles.begin(), innerTiles.end());

		do {
			if ( isCorrect(innerTiles))
			{
				return true;
			}
		} while (std::next_permutation(innerTiles.begin(), innerTiles.end()));

		return false;
	}

	// Get the smallest number from 1 to TileCount, that's
	// not in the outer ring array from b to e, inclusive
	unsigned getSmallest(int b, int e)
	{
		unsigned smallest = 1;
		// Used as a set
		auto candidates = std::array<unsigned, TileCount>();
		for (int i = b; i <= e; ++i)
		{
			candidates[ring[i] - 1] = 1;
		}
		for (size_t i = 0; i < TileCount; ++i)
		{
			if (candidates[i] == 0)
				return i + 1;
		}
		return smallest;
	}

	// Adjust numbers in the outer ring, from idx to the 
	// end of the array, by choosing the smallest unused
	// number.
	void adjust(int idx)
	{
		for (int i = idx; i < RingSize; ++i)
		{
			auto smallest = getSmallest(0, i - 1);
			ring[i] = smallest;
		}
	}

	// Returns true, if number is used in outer ring
	// array from 0 to endIdx, inclusive.
	bool used(unsigned number, int endIdx)
	{
		for (int i = 0; i <= endIdx; ++i)
		{
			if (ring[i] == number)
				return true;
		}
		return false;
	}

	// Gets the next unused number, greater than current
	// Seach the outer ring array from 0 to endIdx inclusive for
	// each potential number.  -1 is returned if we fail to find
	// an unused number <= TileCount
	int getNextNumber(int current, size_t endIdx)
	{
		int nextNumber = current + 1;
		while (nextNumber <= TileCount)
		{
			if (!used(nextNumber, endIdx))
				return nextNumber;
			nextNumber += 1;
		}
		return -1;
	}

	// Find the index of the first tile in the edge containing idx
	int getEdgeIdx(size_t idx)
	{
		if (idx % 2 == 0)
		{
			return idx - 2;
		}
		else
		{
			return idx - idx % 2;
		}
	}

	// Get index of first edge, that doesn't sum to 38
	// Check from 0, to endIdx, inclusive
	int getFirstIncorrectEdge( size_t endIdx)
	{
		// Find earliest edge that's invalid, and start there
		int firstIncorrectEdgeIdx = -1;
		for ( size_t i = 0; i <= endIdx; i += 2)
		{
			if (!isCorrect(i))
			{
				return i;
			}
		}
		return -1;
	}

	bool next(size_t idx)
	{
		if (idx == -1)
			return false;

		// This is key optimization step.  It would take a very long time
		// to find a solution, if we didn't have a way to speed up the 
		// computations.  This avoids looking at most of the permutations.
		// We simply permute starting from the first edge that doesn't sum
		// to 38.
		auto firstIncorrectEdgeIdx = getFirstIncorrectEdge( idx);
		if ( firstIncorrectEdgeIdx != -1 && 
			idx >= firstIncorrectEdgeIdx + 2)
			idx = firstIncorrectEdgeIdx + 2;

		auto nextNumber = getNextNumber(ring[idx], idx - 1);
		if (nextNumber != -1)
		{
			ring[idx] = nextNumber;
			adjust(idx + 1);
			return true;
		}
		else
		{
			return next(idx - 1);
		}
		return false;
	}

	bool next()
	{
		return next(RingSize - 1);
	}

	RingArr ring;
};

int main()
{
	Ring ring; // Outer ring
	InnerTilesArray innerTiles;

	int Limit = 10000000;
	int count = 0;
	while (true)
	{
		if (count % 1 == 0)
		{
			// Soley for debugging/progess
			cout << count << ": ";
			ring.printRing();
		}
		if ( ring.isCorrect()) // Outer ring is correct
		{
			//cout << "Possible solution: " << ring.toString() << endl;
			if ( ring.validInnerTiles( innerTiles))
			{
				cout << "Solution found afer " << count << " iterations.\n";
				cout << "Solution; inner tiles: ";
				std::copy( innerTiles.begin(), innerTiles.end(), std::ostream_iterator<int>(std::cout, " "));
				cout << "\nSolution; outer ring: " << ring.toString() << endl;
				break;
			}
		}
		if (!ring.next())
			break;
		if (++count > Limit)
			break;
	}

	return 0;
}
