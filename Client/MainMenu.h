#ifndef MAINMENU_H
#define MAINMENU_H

#include "State.h"
#include "Client.h"
#include "GameMode.h"

class MainMenu : public State
{
public:
	MainMenu(StateMachine* sm, std::string name, std::string ip);
	~MainMenu();

	void handleInput();
	void update(const float dt);
	void draw(const float dt);

protected:
	void onInitialize();

private:
	Client* client;
	std::string playerName, ipAddress;
};
#endif;