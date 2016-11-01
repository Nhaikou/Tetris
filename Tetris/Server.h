#ifndef SERVER_H
#define SERVER_H

#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

class Server
{
public:
	Server();
	~Server();

	sf::SocketSelector socketSelector;
	sf::TcpSocket tcpSocket;
	sf::TcpListener tcpListener;
	sf::Packet packet;

	void findPlayers();
	
	std::vector<sf::TcpSocket*> clients;
	bool searchPlayers = true;
	char answer;
};
#endif;
