#include "Pathfinder.h"
#include <iostream>
#include <map>
#include <set>
#include <queue>

void Pathfinder::loadMap() {
	// TODO: Load map from .txt file in the future
	std::string tiles = "";
	tiles += "####################";
	tiles += "#R.....#......#....#";
	tiles += "####...#.######....#";
	tiles += "#..................#";
	tiles += "#...###.....##.....#";
	tiles += "#............####..#";
	tiles += "#.......###..#.....#";
	tiles += "#..................#";
	tiles += "#....####....###...#";
	tiles += "#.......#..........#";
	tiles += "#...##.####...###..#";
	tiles += "#....#..#.......##.#";
	tiles += "#.......#...#####..#";
	tiles += "#..................#";
	tiles += "####################";
	for (int y = 0; y < height / tileHeight; y++) {
		for (int x = 0; x < width / tileWidth; x++) {
			char tile = tiles[y * (width / tileWidth) + x];
			nodes.push_back(Node{ x, y, tile });
		}
	}
}

sf::Color Pathfinder::getColor(char tile) {
	// Guaranteed one match
	return std::map<char, sf::Color>{
		{'#', sf::Color::Black},
		{ '.', sf::Color::White },
		{ 'R', sf::Color::Green },
		{ 'P', sf::Color::Magenta },
		{ 'O', sf::Color::Red }}.at(tile);
}

Node Pathfinder::getNode(int x, int y) {
	return nodes[y * (width / tileWidth) + x];
}

void Pathfinder::drawNode(Node node) {
	sf::RectangleShape rect(sf::Vector2f((float)tileWidth - 2, (float)tileHeight - 2));
	rect.setPosition(node.x * tileWidth, node.y * tileHeight);
	rect.setFillColor(getColor(node.tile));
	if (node.tile != '#' && std::get<0>(highlightedTile) == node.x && std::get<1>(highlightedTile) == node.y) {
		// Highlight tile!
		rect.setOutlineColor(sf::Color::Red);
		rect.setOutlineThickness(2);
	}
	else if (std::get<0>(deHighlightedTile) == node.x && std::get<1>(deHighlightedTile) == node.y) {
		// Dehighlight tile!
		rect.setOutlineThickness(0);
	}
	window.draw(rect);
}

void Pathfinder::drawNodes() {
	for (auto& node : nodes) drawNode(node);
}

float Pathfinder::distance(Node nodeA, Node nodeB) {
	return std::sqrt(std::pow(nodeA.x - nodeB.x, 2) + std::pow(nodeA.y - nodeB.y, 2));
}

Node Pathfinder::getNeighbour(int x, int y) {
	return nodes[y * (width / tileWidth) + x];
}

std::vector<Node> Pathfinder::getNeighbours(Node node) {
	int nodeX = node.x;
	int nodeY = node.y;
	std::vector<Node> neighbours;

	// Left?
	if (0 < nodeX - 1 && nodeX - 1 < (width / tileWidth)) {
		Node nb = getNeighbour(nodeX - 1, nodeY);
		if (nb.tile != '#') {
			neighbours.push_back(nb);
		}
	}

	// Right?
	if (0 < nodeX + 1 && nodeX + 1 < (width / tileWidth)) {
		Node nb = getNeighbour(nodeX + 1, nodeY);
		if (nb.tile != '#') {
			neighbours.push_back(nb);
		}
	}
	
	// Top?
	if (0 < nodeY - 1 && nodeY - 1 < (height / tileHeight)) {
		Node nb = getNeighbour(nodeX, nodeY - 1);
		if (nb.tile != '#') {
			neighbours.push_back(nb);
		}
	}
	
	// Bottom?
	if (0 < nodeY + 1 && nodeY + 1 < (height / tileHeight)) {
		Node nb = getNeighbour(nodeX, nodeY + 1);
		if (nb.tile != '#') {
			neighbours.push_back(nb);
		}
	}

	return neighbours;
}

void Pathfinder::resetPath() {
	for (auto& node : nodes) {
		if (node.tile != '#') {
			node.tile = '.';
		}
	}
	drawNodes();
}

std::vector<Node> Pathfinder::reconstructPath(std::map<Node, Node> cameFrom, Node current) {
	std::cout << "Reconstructing path...\n";
	current.tile = 'R';
	nodes[current.y * (width / tileWidth) + current.x] = current;
	std::vector<Node> totalPath;
	totalPath.push_back(current);
	while (cameFrom.find(current) != cameFrom.end()) {
		current = cameFrom.at(current);
		current.tile = 'P';
		nodes[current.y * (width / tileWidth) + current.x] = current;
		totalPath.push_back(current);
	}
	current.tile = 'O';
	nodes[current.y * (width / tileWidth) + current.x] = current;
	return totalPath;
}

std::vector<Node> Pathfinder::astar(Node startNode, Node endNode) {

	std::priority_queue<Node> openSet;
	openSet.push(startNode);
	std::set<Node> closedSet;

	std::map<Node, Node> cameFrom;

	std::map<Node, float> gScore;
	std::map<Node, float> fScore;
	for (auto& node : nodes) {
		if (node.tile == '#') continue;
		if (node.x == startNode.x && node.y == startNode.y) {
			gScore.insert({ startNode, 0 });
			fScore.insert({ startNode, distance(startNode, endNode) });
		}
		else {
			gScore.insert({ node, std::numeric_limits<float>::infinity() });
			fScore.insert({ node, std::numeric_limits<float>::infinity() });
		}
	}

	while (!openSet.empty()) {

		Node current = openSet.top();
		openSet.pop();
		closedSet.insert(current);

		if (current.x == endNode.x && current.y == endNode.y) {
			return reconstructPath(cameFrom, current);
		}

		for (auto& neighbour : getNeighbours(current)) {
			float tentativeGScore = gScore.at(current) + distance(current, neighbour);
			if (tentativeGScore < gScore.at(neighbour)) {
				cameFrom.insert({ neighbour, current });
				gScore.at(neighbour) = tentativeGScore;
				fScore.at(neighbour) = tentativeGScore + distance(neighbour, endNode);
				if (closedSet.find(neighbour) == closedSet.end()) {
					openSet.push(neighbour);
				}
			}
		}

	}

	// No path found
	return std::vector<Node>{};

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
					Node eNode = getNode(x, y);
					if (eNode.tile != '#') {  // Make sure it isn't a wall
						this->endNode = eNode;
						resetPath();
						path = astar(startNode, endNode);
						if (!path.empty()) {
							std::cout << "Found path...\n";
							startNode.tile = '.';
							startNode = endNode;
							startNode.tile = 'R';
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