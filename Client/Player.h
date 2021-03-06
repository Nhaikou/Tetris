#ifndef PLAYER_H
#define PLAYER_H

#include "Board.h"
#include <vector>
#include <sstream>

class Player
{
public:
	Player(sf::Vector2u size, sf::Vector2u spawn, unsigned clientNumber, bool factory);
	~Player();
	
	void updateNextBlock(unsigned blockType);
	void updateScoreText();

	Board* board = nullptr;

	std::stringstream ss;
	unsigned score = 0;
	sf::Font font;
	sf::Text scoreText;
	sf::Texture emptyTex;
	std::vector<std::vector<sf::Sprite>> nextBlockSprites;
	sf::Vector2u nextBlockSize = { 4, 2 }; // The amount of space we need to show our next block
	sf::RectangleShape playerInfoBox;
};
#endif;

