#include "Client.h"


Client::Client()
{
	connectToServer();
}


Client::~Client()
{
	for (int i = 0; i < players.size(); ++i)
	{
		delete players[i];
	}
}

void Client::sendInput(int key)
{
	packet.clear();
	packet << key;
	server.send(packet);
}

void Client::connectToServer()
{
	ip = sf::IpAddress::getLocalAddress();

	server.connect(ip, 55001);

	std::cout << "Waiting for an answer from server...";
	server.receive(packet);

	std::cout << "Continue";
	server.setBlocking(false);

	packet >> playerCount;
}

void Client::standardInitialize()
{
	packet.clear();
	server.receive(packet);
	if (packet.getDataSize() == 0)
	{
		std::cout << "\nERROR: No packets received!!" << std::endl;
		return;
	}

	sf::Vector2u size;
	sf::Vector2u spawn;
	packet >> size.x >> size.y >> spawn.x >> spawn.y;

	for (int i = 0; i < playerCount; ++i)
	{
		bool lastPlayer = false;
		if (i == playerCount - 1)
		{
			lastPlayer = true;
		}
		sf::Vector2i position;
		packet >> position.x >> position.y;
		Player *player = new Player(size, position, spawn, lastPlayer);
		players.push_back(player);
	}
}

void Client::factoryInitialize()
{
	packet.clear();
	server.receive(packet);
	if (packet.getDataSize() == 0)
	{
		std::cout << "\nERROR: No packets received!!" << std::endl;
		return;
	}

	sf::Vector2u size;
	sf::Vector2u spawn;
	packet >> clientNumber >> size.x >> size.y >> spawn.x >> spawn.y;
	for (int i = 0; i < 3; ++i)
	{
		bool lastPlayer = false;
		if (i == 2)
		{
			lastPlayer = true;
		}
		Player *player = new Player(size, sf::Vector2i(i * (size.x + 1) * 16 + 16, -2 * 16), spawn, lastPlayer);
		players.push_back(player);
	}
}

void Client::receiveBoard()
{
	packet.clear();
	server.receive(packet);
	if (packet.getDataSize() == 0)
	{
		return;
	}

	unsigned type, id, i, j;
	packet >> id;
	if (gameMode == true)
	{
		if (id == clientNumber)
		{
			id = 1;
		}
		else if (clientNumber == playerCount - 1 && id == 0)
		{
			id = 2;
		}
		else if (clientNumber == 0 && id == playerCount - 1)
		{
			id = 0;
		}
		else if (id < clientNumber)
		{
			id = 0;
		}
		else if (id > clientNumber)
		{
			id = 2;
		}
	}

	while (!packet.endOfPacket())
	{
		packet >> i >> j >> type;
		if (type == BlockType::EMPTY)
		{
			players[id]->board->grid[i][j].setTexture(players[id]->board->emptyTex);
			players[id]->board->grid[i][j].setColor(sf::Color::White);
		}
		else
		{
			players[id]->board->grid[i][j].setTexture(players[id]->board->blockTex);
			if (type == BlockType::BLOCKI)
			{
				players[id]->board->grid[i][j].setColor(sf::Color::Cyan);
			}
			else if (type == BlockType::BLOCKO)
			{
				players[id]->board->grid[i][j].setColor(sf::Color::Yellow);
			}
			else if (type == BlockType::BLOCKZ)
			{
				players[id]->board->grid[i][j].setColor(sf::Color::Red);
			}
			else if (type == BlockType::BLOCKS)
			{
				players[id]->board->grid[i][j].setColor(sf::Color::Green);
			}
			else if (type == BlockType::BLOCKL)
			{
				players[id]->board->grid[i][j].setColor(sf::Color(255, 130, 0));
			}
			else if (type == BlockType::BLOCKJ)
			{
				players[id]->board->grid[i][j].setColor(sf::Color::Blue);
			}
			else if (type == BlockType::BLOCKT)
			{
				players[id]->board->grid[i][j].setColor(sf::Color::Magenta);
			}
		}
	}
}

bool Client::receiveState()
{
	server.receive(packet);
	if (packet.getDataSize() == 0)
	{
		return false;
	}
	packet >> gameMode;

	return true;
}