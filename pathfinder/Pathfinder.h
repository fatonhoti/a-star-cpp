#pragma once
#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <SFML/Graphics.hpp>
#include <tuple>
#include <vector>

struct Node {
	int x, y;
	char tile;
	float gScore;
	float fScore;
	Node* parent;
	bool inOpenSet = false;
	bool inClosedSet = false;
	bool operator<(const Node& rhs) const {
		return fScore > rhs.fScore;
	}
	friend std::ostream& operator<<(std::ostream& os, const Node& n);
};

class Pathfinder {
private:
	int tileWidth;
	int tileHeight;
	int width;
	int height;
	std::string title;
	std::vector<Node*> nodes;
	std::vector<Node*> path;
	sf::RenderWindow window;
	Node* startNode;
	Node* endNode;
	std::tuple<int, int> deHighlightedTile;
	std::tuple<int, int> highlightedTile;
public:
	void loadMap();
	sf::Color getColor(char);
	Node* getNode(int, int);
	void drawNode(Node*);
	void drawNodes();
	float distance(Node*, Node*);
	Node* getNeighbour(int, int);
	std::vector<Node*> getNeighbours(Node*);
	std::vector<Node*> reconstructPath(Node*);
	void setPath(std::vector<Node>);
	void resetPath();
	std::vector<Node*> astar(Node*, Node*);
	void run();
	Pathfinder();
};

#endif