#pragma once
#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <SFML/Graphics.hpp>
#include <tuple>
#include <vector>

struct Node {
	int x, y;
	char tile;
};

class Pathfinder {
private:
	int tileWidth;
	int tileHeight;
	int width;
	int height;
	std::string title;
	std::vector<Node> nodes;
	sf::RenderWindow window;
	std::tuple<int, int> deHighlightedTile;
	std::tuple<int, int> highlightedTile;
public:
	void loadMap();
	sf::Color getColor(char);
	Node getNode(int, int);
	void drawTile(int, int, char);
	float heuristic(int, int);
	void astar(int, int);
	void run();
	Pathfinder();
};

#endif