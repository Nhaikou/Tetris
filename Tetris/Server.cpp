#include "Server.h"


Server::Server(bool multiplayer)
{
	networking = false;
	if (multiplayer)
	{
		findPlayers();
		networking = true;
	}
	lineTime.y = 2000;
}

Server::~Server()
{
	for (int i = 0; i < clients.size(); ++i)
	{
		delete clients[i];
	}
}

void Server::findPlayers()
{
	tcpListener.listen(55001);
	socketSelector.add(tcpListener);

	while (searchPlayers)
	{
		std::cout << "Waiting for players.." << std::endl;
		if (socketSelector.wait() && searchPlayers)
		{
			if (socketSelector.isReady(tcpListener) && searchPlayers)
			{
				sf::TcpSocket* client = new sf::TcpSocket;
				if (tcpListener.accept(*client) == sf::Socket::Done)
				{
					clients.push_back(client);
					clients[clients.size() - 1]->setBlocking(false);
					socketSelector.add(*client);
					Player *player = new Player(clients.size() - 1, sf::Vector2i(0, -2), sf::Vector2u(10, 20), sf::Vector2u(3, 3));
					players.push_back(player);
				}
				else
				{
					delete client;
				}

				std::cout << "Wait for more players? Y/N";
				std::cin >> answer;
				if (answer == 'n')
				{
					packet << players.size();
					for (int i = 0; i < clients.size(); ++i)
					{
						clients[i]->send(packet);
					}
					searchPlayers = false;
				}
			}
		}
	}
}

int Server::receiveButtonPress(unsigned id)
{
	if (!networking)
	{
		return -1;
	}

	int key;
	clients[id]->receive(packet);
	if (packet.getDataSize() == 0)
	{
		return -1;
	}
	packet >> key;
	return key;
}

void Server::sendBoard(unsigned id)
{
	packet.clear();
	packet << 1; // secretCode
	packet << id;

	for (int j = 0; j < players[id]->board->getSize().y; ++j)
	{
		for (int i = 0; i < players[id]->board->getSize().x; ++i)
		{
			if (players[id]->board->updatedGrid[i][j] != players[id]->board->grid[i][j])
			{
				packet << i << j << players[id]->board->grid[i][j];
			}
		}
	}
	for (int i = 0; i < clients.size(); ++i)
	{
		clients[i]->send(packet);
	}
}

void Server::updateLine(const float dt)
{
	lineTime.x += dt;
	if (lineTime.x >= lineTime.y)
	{
		lineTime.x = 0;
		for (int i = 0; i < players.size(); ++i)
		{
			players[i]->board->updatedGrid = players[i]->board->grid;
		}
		if (lineDirection == -1)
		{
			moveLineLeft();
		}
		else
		{
			moveLineRight();
		}
		for (int i = 0; i < players.size(); ++i)
		{
			if (players[i]->board->clearRow() > 0)
			{
				for (int j = players[i]->currentBlock->positions.size() - 1; j >= 0; --j)
				{
					players[i]->board->grid[players[i]->currentBlock->positions[j].x][players[i]->currentBlock->positions[j].y + 1] = BlockType::EMPTY;
					players[i]->board->grid[players[i]->currentBlock->positions[j].x][players[i]->currentBlock->positions[j].y] = players[i]->currentBlock->getType();
				}
			}
			sendBoard(i);
		}
	}
}

void Server::moveLineLeft()
{

}

void Server::moveLineRight()
{
	std::vector<std::vector<unsigned>> tempSlices;
	for (int i = 0; i < players.size(); ++i)
	{
		std::vector<unsigned> tempSlice = players[i]->board->gridSlice;
		tempSlices.push_back(tempSlice);
	}

	for (int k = 0; k < players.size(); ++k)
	{
		for (int i = 0; i < players[k]->currentBlock->positions.size(); ++i)
		{
			players[k]->board->grid[players[k]->currentBlock->positions[i].x][players[k]->currentBlock->positions[i].y] = BlockType::EMPTY;
		}

		for (int j = 0; j < players[k]->board->getSize().y; ++j)
		{
			for (int i = players[k]->board->getSize().x - 1; i >= 0; --i)
			{
				if (i == players[k]->board->getSize().x - 1)
				{
					players[k]->board->gridSlice[j] = players[k]->board->grid[i][j];
				}
				if (i == 0)
				{
					if (k == 0 && players.size() != 1)
					{
						players[k]->board->grid[i][j] = players[players.size() - 1]->board->gridSlice[j];
					}
					else if (k == 0 && players.size() == 1)
					{
						players[k]->board->grid[i][j] = tempSlices[k][j];
					}
					else
					{
						players[k]->board->grid[i][j] = tempSlices[k - 1][j];
					}
				}
				else
				{
					players[k]->board->grid[i][j] = players[k]->board->grid[i - 1][j];
				}
			}
		}

		bool moveBlock = false, blockWrapped = false;
		for (int i = 0; i < players[k]->currentBlock->positions.size(); ++i)
		{
			if (players[k]->board->grid[players[k]->currentBlock->positions[i].x][players[k]->currentBlock->positions[i].y] != BlockType::EMPTY)
			{
				moveBlock = true;
			}
		}

		if (moveBlock)
		{
			for (int i = 0; i < players[k]->currentBlock->positions.size(); ++i)
			{
				players[k]->currentBlock->positions[i].x++;
				if (players[k]->currentBlock->positions[i].x >= players[k]->board->getSize().x)
				{
					blockWrapped = true;
				}
			}
		}
		if (blockWrapped)
		{
			unsigned dropCount = 0, currentDropCount = players[k]->board->getSize().y;
			for (int i = 0; i < players[k]->currentBlock->positions.size(); ++i)
			{
				dropCount = 0;
				if (players[k]->currentBlock->positions[i].x >= players[k]->board->getSize().x)
				{
					while (players[k]->currentBlock->positions[i].y + dropCount != players[k]->board->getSize().y - 1 && players[k]->board->gridSlice[players[k]->currentBlock->positions[i].y + dropCount + 1] == BlockType::EMPTY)
					{
						dropCount++;
					}
				}
				else
				{
					while (players[k]->currentBlock->positions[i].y + dropCount != players[k]->board->getSize().y - 1 && players[k]->board->grid[players[k]->currentBlock->positions[i].x][players[k]->currentBlock->positions[i].y + dropCount + 1] == BlockType::EMPTY)
					{
						dropCount++;
					}
				}
				if (dropCount < currentDropCount)
				{
					currentDropCount = dropCount;
				}
			}

			for (int i = 0; i < players[k]->currentBlock->positions.size(); ++i)
			{
				for (int j = 0; j < currentDropCount; ++j)
				{
					if (players[k]->currentBlock->positions[i].x >= players[k]->board->getSize().x)
					{
						players[k]->board->gridSlice[players[k]->currentBlock->positions[i].y + currentDropCount] = players[k]->currentBlock->getType();
					}
					else
					{
						players[k]->board->grid[players[k]->currentBlock->positions[i].x][players[k]->currentBlock->positions[i].y + currentDropCount] = players[k]->currentBlock->getType();
					}
				}
			}

			players[k]->spawnBlock();
		}
		else
		{
			for (int i = 0; i < players[k]->currentBlock->positions.size(); ++i)
			{
				players[k]->board->grid[players[k]->currentBlock->positions[i].x][players[k]->currentBlock->positions[i].y] = players[k]->currentBlock->getType();
			}
		}
	}
}