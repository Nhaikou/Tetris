#ifndef STANDARD_H
#define STANDARD_H

#include "State.h"
#include "Board.h"
#include "Block.h"
#include "BlockI.h"
#include "BlockO.h"
#include "BlockZ.h"
#include "BlockS.h"
#include "BlockL.h"
#include "BlockJ.h"
#include "BlockT.h"
#include "Bag.h"

#include <sstream>

class Standard : public State
{
public:
	Standard(StateMachine* sm);
	~Standard();

	void handleInput();
	void update(const float dt);
	void draw(const float dt);

	void spawnBlock();

protected:
	void onInitialize();

private:
	Board *board = nullptr;
	Block *currentBlock = nullptr;
	Bag bag;
	sf::Font font;
	sf::Text scoreText;
	std::stringstream ss;
};

#endif