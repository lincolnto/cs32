#include "provided.h"
#include "HashTable.h"
#include <string>
#include <vector>
using namespace std;

void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
	unsigned int capacity, x = s.size() / 2 + 512;
	if (x < 16384) // Initialize capacity
		capacity = x;
	else
		capacity = 16384;
	// HashTable H(;
	  // This compiles, but may not be correct
}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
	return false;  // This compiles, but may not be correct
}
