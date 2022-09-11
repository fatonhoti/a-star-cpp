#include "Pathfinder.h"
#include <iostream>
#include <map>
#include <set>
#include <queue>

std::ostream& operator<<(std::ostream& os, const Node& n)
{
	return os << "(" << n.x << ", " << n.y << ", " << n.tile << ")\n";
}

void Pathfinder::loadMap() {
	// TODO: Load map from .txt file in the future
	std::string tiles = "";
	tiles += "####################";
	tiles += "#R.....#......#....#";
	tiles += "####...#.######....#";
	tiles += "#..................#";
	tiles += "#######.....##.....#";
	tiles += "#............####..#";
	tiles += "#.#.....######.....#";
	tiles += "#.#................#";
	tiles += "#.#..#######.###...#";
	tiles += "#.#.....#..........#";
	tiles += "#...##.####...###..#";
	tiles += "#....#..#.......##.#";
	tiles += "#.##....#...#####..#";
	tiles += "#..#...............#";
	tiles += "####################";
	for (int y = 0; y < height / tileHeight; y++) {
		for (int x = 0; x < width / tileWidth; x++) {
			char tile = tiles[y * (width / tileWidth) + x];
			Node* n = new Node();
			n->x = x;
			n->y = y;
			n->tile = tile;
			nodes.push_back(n);
		}
	}
}

sf::Color Pathfinder::getColor(char tile) {
	return std::map<char, sf::Color>{
		{'#', sf::Color::Black},
		{ '.', sf::Color::White },
		{ 'R', sf::Color::Green },
		{ 'P', sf::Color::Magenta },
		{ 'O', sf::Color::Red }}.at(tile);
}

Node* Pathfinder::getNode(int x, int y) {
	return nodes[y * (width / tileWidth) + x];
}

void Pathfinder::drawNode(Node* node) {
	sf::RectangleShape rect(sf::Vector2f((float)tileWidth - 2, (float)tileHeight - 2));
	rect.setPosition(node->x * tileWidth, node->y * tileHeight);
	rect.setFillColor(getColor(node->tile));
	if (node->tile != '#' && std::get<0>(highlightedTile) == node->x && std::get<1>(highlightedTile) == node->y) {
		// Highlight tile!
		rect.setOutlineColor(sf::Color::Red);
		rect.setOutlineThickness(2);
	}
	else if (std::get<0>(deHighlightedTile) == node->x && std::get<1>(deHighlightedTile) == node->y) {
		// Dehighlight tile!
		rect.setOutlineThickness(0);
	}
	window.draw(rect);
}

void Pathfinder::drawNodes() {
	for (auto& node : nodes) drawNode(node);
}

float Pathfinder::distance(Node* nodeA, Node* nodeB) {
	return std::sqrt(std::pow(nodeA->x - nodeB->x, 2) + std::pow(nodeA->y - nodeB->y, 2));
}

Node* Pathfinder::getNeighbour(int x, int y) {
	return nodes[y * (width / tileWidth) + x];
}

std::vector<Node*> Pathfinder::getNeighbours(Node* node) {
	int nodeX = (*node).x;
	int nodeY = (*node).y;
	std::vector<Node*> neighbours;

	// Left?
	if (0 < nodeX - 1 && nodeX - 1 < (width / tileWidth)) {
		Node* nb = getNeighbour(nodeX - 1, nodeY);
		if ((*nb).tile != '#') {
			neighbours.push_back(nb);
		}
	}

	// Right?
	if (0 < nodeX + 1 && nodeX + 1 < (width / tileWidth)) {
		Node* nb = getNeighbour(nodeX + 1, nodeY);
		if ((*nb).tile != '#') {
			neighbours.push_back(nb);
		}
	}
	
	// Top?
	if (0 < nodeY - 1 && nodeY - 1 < (height / tileHeight)) {
		Node* nb = getNeighbour(nodeX, nodeY - 1);
		if ((*nb).tile != '#') {
			neighbours.push_back(nb);
		}
	}
	
	// Bottom?
	if (0 < nodeY + 1 && nodeY + 1 < (height / tileHeight)) {
		Node* nb = getNeighbour(nodeX, nodeY + 1);
		if ((*nb).tile != '#') {
			neighbours.push_back(nb);
		}
	}

	return neighbours;
}

void Pathfinder::resetPath() {
	for (auto& node : nodes) {
		if (node->tile != '#') {
			node->tile = '.';
			node->fScore = 0;
			node->gScore = 0;
			node->parent = nullptr;
			node->inOpenSet = false;
			node->inClosedSet = false;
		}
	}
	drawNodes();
}

std::vector<Node*> Pathfinder::reconstructPath(Node* current) {
	std::cout << "Reconstructing path...\n";
	current->tile = 'R';
	std::vector<Node*> totalPath;
	totalPath.push_back(current);
	while (current->parent != nullptr) {
		current = current->parent;
		if (current->parent == nullptr) {
			current->tile = 'O';
		}
		else {
			current->tile = 'P';
		}
		totalPath.push_back(current);
	}
	return totalPath;
}

std::vector<Node*> Pathfinder::astar(Node* startNode, Node* endNode) {

	std::priority_queue<Node*> openSet;
	openSet.push(startNode);
	for (auto& node : nodes) {
		if (node->tile == '#') continue;
		if (node->x == startNode->x && node->y == startNode->y) {
			startNode->gScore = 0;
			startNode->fScore = startNode->gScore + distance(startNode, endNode);
		}
		else {
			node->gScore = std::numeric_limits<float>::infinity();
			node->fScore = std::numeric_limits<float>::infinity();
		}
	}


	while (!openSet.empty()) {

		Node* current = openSet.top();
		current->inClosedSet = true;
		openSet.pop();

		if (current->x == endNode->x && current->y == endNode->y) {
			return reconstructPath(current);
		}

		for (auto& neighbour : getNeighbours(current)) {
			if (neighbour->inClosedSet)
				continue;
			float tentativeGScore = current->gScore + distance(current, neighbour);
			if (tentativeGScore < neighbour->gScore) {
				neighbour->parent = current;
				neighbour->gScore = tentativeGScore;
				neighbour->fScore = tentativeGScore + distance(neighbour, endNode);
				if (!neighbour->inOpenSet) {
					openSet.push(neighbour);
					neighbour->inOpenSet = true;
				}
			}
		}

	}

	// No path found
	return std::vector<Node*>{};

}

void Pathfinder::run() {

	while (window.isOpen()) {
		sf::Event event;
		if (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}

			else if (event.type == sf::Event::MouseMoved) {
				int x = event.mouseMove.x / tileWidth;
				int y = event.mouseMove.y / tileHeight;
				int highlightedX = std::get<0>(highlightedTile);
				int highlightedY = std::get<1>(highlightedTile);
				if (highlightedX != x || highlightedY != y) {
					if (std::get<0>(highlightedTile) != -1) {
						deHighlightedTile = highlightedTile;
					}
					highlightedTile = std::make_tuple(x, y);
				}
			}

			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					int x = event.mouseButton.x / tileHeight;
					int y = event.mouseButton.y / tileWidth;
					Node* eNode = getNode(x, y);
					if (eNode->tile != '#') {  // Make sure it isn't a wall
						this->endNode = eNode;
						resetPath();
						path = astar(startNode, endNode);
						if (!path.empty()) {
							std::cout << "Found path...\n";
							startNode = endNode;
							startNode->tile = 'R';
						}
						else {
							std::cout << "Failed to find a path...\n";
						}
					}
				}
			}
		}

		window.clear();
		drawNodes();
		window.display();

	}

}

Pathfinder::Pathfinder() {
	width = 800;
	height = 600;
	tileWidth = 40;
	tileHeight = tileWidth;
	title = "Pathfinder";
	loadMap();
	window.create(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close);
	startNode = nodes[1 * (width / tileWidth) + 1];
	deHighlightedTile = std::make_tuple(-1, -1);
	highlightedTile = std::make_tuple(-1, -1);
}