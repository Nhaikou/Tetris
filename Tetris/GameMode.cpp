#include "GameMode.h"


GameMode::GameMode(StateMachine* sm, Server *srvr, bool factory)
{
	server = srvr;
	stateMachine = sm;
	factoryMode = factory;
}


GameMode::~GameMode()
{

}

void GameMode::onInitialize()
{
	server->gameModeInitialize(factoryMode);

	spawnUpdate();
	for (int i = 0; i < server->players.size(); ++i)
	{
		if (!server->players[i]->spawnBlock())
		{
			server->boardFull(i);
		}
	}

	for (int i = 0; i < server->clients.size(); ++i)
	{
		server->sendBoard(i);
	}

	endTimer = sf::Vector2f(0, 300000);
}

void GameMode::update(const float dt)
{
	endTimer.x += dt;
	/*
	if (endTimer.x >= endTimer.y)
	{
		server->endGame();
		stateMachine->popState();
	}
	*/

	for (int i = 0; i < server->clients.size(); ++i)
	{
		if (!server->players[i]->playerOut)
		{
			server->players[i]->clientKey = server->receiveButtonPress(i);

			if (server->players[i]->clientKey != -1)
			{
				server->players[i]->updateClient();
				server->sendBoard(i);
				server->players[i]->clientKey = -1;
			}

			if (server->players[i]->dropUpdate(dt))
			{
				server->sendBoard(i);
			}
		}
	}

	if (factoryMode)
	{
		server->updateLine(dt);
	}

	for (int i = 0; i < server->clients.size(); ++i)
	{
		if (server->players[i]->fullBoard)
		{
			server->players[i]->fullBoard = false;
			server->boardFull(i);
		}
	}

	spawnUpdate();
}

void GameMode::spawnUpdate()
{
	unsigned smallestNumber = server->players[0]->currentBlockId;

	for (int i = 0; i < server->clients.size(); ++i)
	{
		if (server->players[i]->blockSpawned)
		{
			server->players[i]->blockSpawned = false;
			if (server->players[i]->currentBlockId >= server->bags.size())
			{
				server->newBag();
			}
			server->players[i]->nextBlock = server->bags[server->players[i]->currentBlockId];
			server->players[i]->currentBlockId++;
			server->sendNextBlock(i);
			server->sendScore(i);
		}

		if (server->players[i]->currentBlockId < smallestNumber)
		{
			smallestNumber = server->players[i]->currentBlockId;
		}
	}

	for (int i = 0; i < smallestNumber; ++i)
	{
		server->bags.erase(server->bags.begin());
	}

	for (int i = 0; i < server->clients.size(); ++i)
	{
		server->players[i]->currentBlockId -= smallestNumber;
	}
}