#include "provided.h"
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut)
{
	if (hostIn.size() == 0) { cout << "HostIn is Empty" << endl; return false; } // hostIn is empty
	vector<string> lines; // Create a vector to store each line
	string s = "";
	for (unsigned int k = 0; k < hostIn.size(); k++)
	{
		if (hostIn[k] != '\n' && hostIn[k] != '\r') // Newline not found, add nonspace/nontab chars
			s += hostIn[k];
		else if (hostIn[k] == '\n' || (hostIn[k] == '\r' && hostIn[k + 1] == '\n'))
		{
			lines.push_back(s);
			s = "";
			if (hostIn[k] == '\r')	k++; // Need to move 2 chars ahead
		}
	}
	if (!s.empty()) lines.push_back(s); // Check if hostIn doesn't end with \n

	//cout << "lsize: " << lines.size();
	for (unsigned int k = 0; k < lines.size(); k++)
	{
		string s = lines[k];
		if (s.size() <= 0) continue;
		for (int j = s.size() - 1; j != 0; j--) // strip trailing spaces/tabs
		{
			if (s[j] == ' ' || s[j] == '\t')
				s[j] = '\0';
			else break;
		}
		lines[k] = s; // Replace string with stripped version
	}

	vector<unsigned short> nums;
	//cout << "msg " << msg << endl;
	Compressor::compress(msg, nums);
	//cout << "nums size: " << nums.size() << endl;
	string encodedMsg = BinaryConverter::encode(nums);
	//cout << "Enc msg size: " << encodedMsg.size() << endl;

	int N = lines.size();
	int L = encodedMsg.size();
	int firstStrings = L % N, firstLen = (L / N) + 1;
	int lastStrings = N - (L % N), lastLen = L / N;
	vector<string> substrings;

	int x = 0;
	for (int k = 0; k < lines.size(); k++)
	{
		int numCharsToAdd = firstStrings;
		if (k < firstStrings) numCharsToAdd++;
		string str = "";
		for (int i = 0; i < numCharsToAdd; i++)
		{
			str += encodedMsg[x];
			x++;
		}
		substrings.push_back(str);
	}
	
	int subsize = 0;
	for (int k = 0; k < substrings.size(); k++)
	{
		subsize += substrings[k].size();
	}
	cout << "subsize: " << subsize << endl;
	// REMOVED CODE HERE //

	hostOut.clear();
	for (unsigned int k = 0; k < lines.size(); k++) // create hostOut string
	{
		//cout << "lol" << endl;
		hostOut += lines[k];
		if (k < substrings.size())
			hostOut += substrings[k];
		hostOut += '\n';
	}
	return true; 
}

bool Steg::reveal(const string& hostIn, string& msg)
{
	if (hostIn.size() == 0) return false; // hostIn is empty
	vector<string> lines; // Create a vector to store each line
	string s = "";
	for (unsigned int k = 0; k < hostIn.size(); k++)
	{
		if (hostIn[k] != '\n' && hostIn[k] != '\r') // Newline not found, add nonspace/nontab chars
			s += hostIn[k];
		else if (hostIn[k] == '\n' || (hostIn[k] == '\r' && hostIn[k + 1] == '\n'))
		{
			//s += "NEWLINE";
			lines.push_back(s);
			//cout << "ssize: " << s.size() << endl;
			s = "";
			if (hostIn[k] == '\r')	k++; // Need to move 2 chars ahead
		}
	}
	if (!s.empty()) lines.push_back(s); // Check if hostIn doesn't end with \n

	//for (int k = 0; k < lines.size(); k++)
	//{
	//	for (int j = 0; j < lines[k].size(); j++)
	//	{
	//		if (lines[k][j] == ' ') cout << '_';
	//		else cout << '-';
	//	}
	//}
	string encodedMsg = "";
	//cout << "Lines size: " << lines.size() << endl;
	//for (int k = 0; k < lines.size(); k++)
	//	cout << lines[k] << endl;
	for (int k = 0; k < lines.size(); k++)
	{
		string s = lines[k];
		// cout << "lines[" << k << "] size: " << s.size() << endl;
		string str = "";
		if (s.size() == 0) continue;
		for (int j = s.size() - 1; j >= 0; j--)
		{
			if (s[j] != ' ' && s[j] != '\t')
			{
				s.erase(s.begin(), s.begin() + j + 1);
				break;
			}
		}
		lines[k] = s;
	}
	for (unsigned int k = 0; k < lines.size(); k++)
	{
		encodedMsg += lines[k];
	}
	//for (int k = 0; k < encodedMsg.size(); k++)
	//{
	//	if (encodedMsg[k] == ' ') cout << '_';
	//	else cout << '-';
	//}
	//cout << endl;
	vector<unsigned short> nums;
	//cout << "encodedmsg size: " << encodedMsg.size() << endl;
	if (!BinaryConverter::decode(encodedMsg, nums))
	{
		cout << "Decoding failed!" << endl;
		return false; // Decoding failed
	}
	string decodedMsg;
	if (!Compressor::decompress(nums, decodedMsg))
	{
		cout << "Decompressing failed!" << endl;
		return false; // Decompressing failed
	}
	msg = decodedMsg;
	return true;
}
