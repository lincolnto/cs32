#include "provided.h"
#include <string>
#include <vector>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

#include <iostream>
string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
	if (numbers.size() == 0) // Check for empty vector
		return "";
	string binaryNums = ""; // create a string to store binary conversions
	for (int k = 0; k != numbers.size(); k++) // Begin converting numbers
	{
		// cout << numbers[k] << " ";
		binaryNums += convertNumberToBitString(numbers[k]);
	}

	string encodedMsg = "";
	for (int k = 0; k != binaryNums.size(); k++) // Convert binary string
	{
		if (binaryNums[k] == '0')
			encodedMsg += " ";
		else if (binaryNums[k] == '1')
			encodedMsg += "\t";
	}
	return encodedMsg;
}

bool BinaryConverter::decode(const string& bitString,
	vector<unsigned short>& numbers)
{
	numbers.clear(); // Clear the vector
	if (bitString.size() % 16 != 0) // Check if string contains multiple of 16 chars
	{
		cout << "ERROR! Not multiple of 16!" << endl;
		return false;
	}
	if (bitString.size() == 0) // Empty bit string
		return true;

	int curPos = 0;
	string binaryString = "";
	for (int k = 0; k < bitString.size(); k++) // Convert bitString to binary
	{
		if (bitString[k] != ' ' && bitString[k] != '\t') // Check if string contains chars other than " " or "\t"
		{
			cout << "BitString contains chars other than binary" << endl;
			return false;
		}
		else
		{
			if (bitString[k] == ' ')
				binaryString += '0';
			else
				binaryString += '1';
			curPos++;
			//cout << "binStr: " << binaryString << endl;
		}
		if (curPos == 16)  // Convert binary, then clear string
		{
		//	cout << "binString: " << binaryString << endl;
			unsigned short num;
			convertBitStringToNumber(binaryString, num);
		//	cout << "num: " << num << endl;
			numbers.push_back(num);
			curPos = 0;
			binaryString = "";
		}
	}
	return true;
}

string convertNumberToBitString(unsigned short number)
{
	string result(BITS_PER_UNSIGNED_SHORT, '0');
	for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
	{
		k--;
		if (number % 2 == 1)
			result[k] = '1';
	}
	return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
	if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
		return false;
	number = 0;
	for (size_t k = 0; k < bitString.size(); k++)
	{
		number *= 2;
		if (bitString[k] == '1')
			number++;
		else if (bitString[k] != '0')
			return false;
	}
	return true;
}
