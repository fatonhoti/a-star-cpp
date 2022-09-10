#pragma once
#ifndef PATHFINDER_H
#define PATHFINDER_H
#include <SFML/Graphics.hpp>

class Pathfinder {
private:
	int tileWidth;
	int tileHeight;
	int width;
	int height;
	std::string title;
	std::string tiles;
	sf::RenderWindow window;
public:
	std::string loadMap();
	char getTile(int, int);
	void setTile(int, int, char);
	void drawTile(int, int, char);
	void run();
	Pathfinder();
};

#endif