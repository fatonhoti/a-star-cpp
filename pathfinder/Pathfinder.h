#pragma once
#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <SFML/Graphics.hpp>
#include <tuple>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <queue>

constexpr auto WALL = '#';
constexpr auto EMPTY = '.';
constexpr auto ROBOT = 'R';
constexpr auto PATH = 'P';
constexpr auto CAMEFROM = 'O';

struct Node {
	int x, y;
	char tile;
	float gScore, fScore;
	Node* parent;
	bool hasBeenConsidered, hasBeenVisited;
	bool operator>(const Node& rhs) const {
		return fScore > rhs.fScore;
	}
	friend std::ostream& operator<<(std::ostream& os, const Node& n);
};

struct Tile {
	int x, y;
};

class Pathfinder {
private:
	int width;
	int height;
	int tileWidth;
	int tileHeight;
	int hTiles;
	int vTiles;
	std::vector<Node*> nodes;
	std::vector<Node*> path;
	sf::RenderWindow window;
	sf::Vector2f rectangleSize;
	Node* startNode;
	Node* endNode;
	Tile highlightedTile;
public:
	void loadTileMap();
	sf::Color getColor(char);
	Node* getNode(int, int);
	void drawNode(Node*);
	void drawNodes();
	float heuristic(Node*, Node*);
	Node* getNeighbour(int, int);
	std::vector<Node*> getNeighbours(Node*);
	std::vector<Node*> reconstructPath(Node*);
	void setPath(std::vector<Node>);
	void resetNodes();
	std::vector<Node*> astar(Node*, Node*);
	void run();
	Pathfinder();
};

#endif