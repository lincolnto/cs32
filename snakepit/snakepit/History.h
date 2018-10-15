#ifndef HISTORY_H
#define HISTORY_H

#include "globals.h"
class Pit;
class History
{
public:
	History(int nRows, int nCols);
	bool record(int r, int c);
	void display() const;
private:
	Pit* m_pit;
	int m_rows;
	int m_cols;
};

#endif //  HISTORY_H  //