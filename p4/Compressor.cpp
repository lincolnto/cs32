#include "provided.h"
#include "HashTable.h"
#include <string>
#include <vector>
#include <sstream>
using namespace std;

unsigned int computeHash(string key)
{
	unsigned int i, total = 0;
	for (i = 0; i < key.length(); i++)
		total = total + (i + 1) * key[i];
	return total;
}
unsigned int computeHash(unsigned short key)
{
	return key;
}

#include <iostream>

void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
	// cout << "string s:" << endl << s << endl;
	unsigned int capacity, x = (s.size() / 2) + 512;
	if (x < 16384) // Initialize capacity
		capacity = x;
	else
		capacity = 16384;
	unsigned int numBuckets = capacity * 2; // Load factor of 0.5
	HashTable<string, unsigned short> H(numBuckets, capacity);

	for (int j = 0; j < 256; j++) // Map every possible char to unsigned shorts
	{
		char c = static_cast<char> (j);
		string str = "";	str += c;
		H.set(str, j, true);
		// cout << "j: " << j << " str: " << str << endl;
	}

	int nextFreeID = 256;
	string runSoFar = "";
	vector<unsigned short> V;
	for (unsigned int k = 0; k < s.size(); k++)
	{
		//cout << "s[" << k << "]: " << s[k] << endl;
		unsigned short x;	stringstream ss;	string c;
		ss << s[k]; ss >> c;
		// cout << "c: " << c << endl;
		string expandedRun = runSoFar + c;
		if (H.get(expandedRun, x)) // Expanded run already contained in H
		{
			runSoFar = expandedRun;
			// cout << " runSoFar: " << runSoFar << endl;
			continue;
		}
		else
		{
			H.get(runSoFar, x);
			V.push_back(x);
			H.touch(runSoFar);
			runSoFar = "";
			unsigned short cv;
			H.get(c, cv);
			V.push_back(cv);
			if (!H.isFull())
			{
				H.set(expandedRun, nextFreeID);
				nextFreeID++;
			}
			else
			{
				string discardedKey;	unsigned short discardedValue;
				H.discard(discardedKey, discardedValue);
				H.set(expandedRun, discardedValue);
			}
		}
	}
	if (!runSoFar.empty())
	{
		unsigned short x;
		H.get(runSoFar, x);
		V.push_back(x);
	}
	V.push_back(capacity);

	numbers.clear();
	numbers = V;
}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{

	//cout << "Numbers: " << numbers.size() << endl;
	int capacity = numbers.back();
	int numBuckets = capacity * 2;
	HashTable<unsigned short, string> H(numBuckets, capacity);
	for (int j = 0; j < 256; j++) // Map every possible char to unsigned shorts
	{
		char c = static_cast<char> (j);
		string str = "";	str += c;
		H.set(j, str, true);
		//cout << "j: " << j << " str: " << str << endl;
	}
	int nextFreeID = 256;

	string runSoFar = "", output = "";
	/*for (int k = 0; k < numbers.size(); k++)
		cout << numbers[k] << " ";*/
	for (unsigned int k = 0; k < numbers.size() - 1; k++)
	{
		unsigned short us = numbers[k];
		//cout << "us: " << us << endl;
		if (us <= 255) // us represents a normal 1byte char
		{
			string str;
			H.get(us, str);	output += str;
			if (runSoFar.empty())
			{
				runSoFar = str;
				continue;
			}
			else
			{
				string expandedRun = runSoFar + str;
			//	cout << "expandedRun: " << expandedRun << endl;
				if (!H.isFull())
				{
					H.set(nextFreeID, expandedRun);
					nextFreeID++;
				}
				else
				{
					unsigned short discardedKey; string discardedValue;
					H.discard(discardedKey, discardedValue);
					H.set(discardedKey, expandedRun);
				}
				runSoFar = "";
				continue;
			}
		}
		else
		{
			string str;
			if (!H.get(us, str)) // Error found!
				return false;
			H.touch(us);
			output += str;
			runSoFar = str;
			continue;
		}
	}
	s = output;
	return true;
}