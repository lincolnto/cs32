#ifndef PIT_H
#define PIT_H

#include "globals.h"
#include "Player.h"
#include "Snake.h"
#include <iostream>
#include "History.h"
using namespace std;

class Pit
{
public:
	// Constructor/destructor
	Pit(int nRows, int nCols);
	~Pit();

	// Accessors
	int     rows() const;
	int     cols() const;
	Player* player() const;
	int     snakeCount() const;
	int     numberOfSnakesAt(int r, int c) const;
	void    display(string msg) const;
	History& history();

	// Mutators
	bool   addSnake(int r, int c);
	bool   addPlayer(int r, int c);
	bool   destroyOneSnake(int r, int c);
	bool   moveSnakes();

private:
	History m_history;
	int     m_rows;
	int     m_cols;
	Player* m_player;
	Snake*  m_snakes[MAXSNAKES];
	int     m_nSnakes;
};
#endif //  PIT_H  //