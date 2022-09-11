#include "Pathfinder.h"

std::ostream& operator<<(std::ostream& os, const Node& n)
{
	return os << "(" << n.x << ", " << n.y << ", " << n.tile << ")\n";
}

void Pathfinder::loadTileMap() {
	// TODO: Load map from .txt file in the future
	std::string tiles = "";
	tiles += "####################";
	tiles += "#R...#.#......#....#";
	tiles += "####.#.#.######....#";
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
	for (int y = 0; y < vTiles; y++) {
		for (int x = 0; x < hTiles; x++) {
			char tile = tiles[y * hTiles + x];
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
		{WALL, sf::Color::Black},
		{EMPTY, sf::Color::White },
		{ROBOT, sf::Color::Green },
		{PATH, sf::Color::Magenta },
		{CAMEFROM, sf::Color::Red }}.at(tile);
}

Node* Pathfinder::getNode(int x, int y) {
	return nodes[y * hTiles + x];
}

void Pathfinder::drawNode(Node* node) {
	sf::RectangleShape rect(rectangleSize);
	rect.setPosition(node->x * tileWidth, node->y * tileHeight);
	rect.setFillColor(getColor(node->tile));
	if (node->tile != WALL && highlightedTile.x == node->x && highlightedTile.y == node->y) {
		// Highlight tile!
		rect.setOutlineColor(sf::Color::Red);
		rect.setOutlineThickness(2);
	}
	window.draw(rect);
}

void Pathfinder::drawNodes() {
	for (auto& node : nodes) drawNode(node);
}

Node* Pathfinder::getNeighbour(int x, int y) {
	return nodes[y * hTiles + x];
}

std::vector<Node*> Pathfinder::getNeighbours(Node* node) {

	// Only considering 4 moves, left-right and top-bottom

	int x = node->x;
	int y = node->y;
	std::vector<Node*> neighbours;

	// Left?
	if (0 < x - 1 && x - 1 < hTiles) {
		Node* nb = getNeighbour(x - 1, y);
		if (nb->tile != WALL) {
			neighbours.push_back(nb);
		}
	}

	// Right?
	if (0 < x + 1 && x + 1 < hTiles) {
		Node* nb = getNeighbour(x + 1, y);
		if (nb->tile != WALL) {
			neighbours.push_back(nb);
		}
	}
	
	// Top?
	if (0 < y - 1 && y - 1 < vTiles) {
		Node* nb = getNeighbour(x, y - 1);
		if (nb->tile != WALL) {
			neighbours.push_back(nb);
		}
	}
	
	// Bottom?
	if (0 < y + 1 && y + 1 < vTiles) {
		Node* nb = getNeighbour(x, y + 1);
		if (nb->tile != WALL) {
			neighbours.push_back(nb);
		}
	}

	return neighbours;
}

void Pathfinder::resetNodes() {
	for (auto& node : nodes) {
		if (node->tile != WALL) {
			node->tile = EMPTY;
			node->fScore = 0.0;
			node->gScore = 0.0;
			node->parent = nullptr;
			node->hasBeenConsidered = false;
			node->hasBeenVisited = false;
		}
	}
}

float Pathfinder::heuristic(Node* nodeA, Node* nodeB) {
	return std::abs(nodeA->x - nodeB->x) + std::abs(nodeA->y - nodeB->y);
	//return std::sqrt(std::pow(nodeA->x - nodeB->x, 2) + std::pow(nodeA->y - nodeB->y, 2));
}

std::vector<Node*> Pathfinder::reconstructPath(Node* current) {
	std::vector<Node*> totalPath;
	totalPath.push_back(current);
	current->tile = ROBOT;
	// Backtrack to starting node
	while (current->parent) {
		current = current->parent;
		current->tile = current->parent == nullptr ? CAMEFROM : PATH;
		totalPath.push_back(current);
	}
	return totalPath;
}

std::vector<Node*> Pathfinder::astar(Node* startNode, Node* endNode) {

	// openSet stores the nodes which we might be interested in further looking at
	// openSet is a min-pq for better performance when extracting node with minimum fScore
	std::priority_queue<Node*> openSet;
	for (auto& node : nodes) {
		if (node->tile != WALL) {
			node->gScore = std::numeric_limits<float>::infinity();
			node->fScore = std::numeric_limits<float>::infinity();
		}
	}
	startNode->gScore = 0;
	startNode->fScore = /* startNode.gScore + */ heuristic(startNode, endNode);

	openSet.push(startNode);
	while (!openSet.empty()) {

		Node* current = openSet.top();
		current->hasBeenVisited = true;
		openSet.pop();

		if (current == endNode)
			// We have found a path to the end node!
			return reconstructPath(current);

		for (auto& neighbour : getNeighbours(current)) {
			if (!neighbour->hasBeenVisited) {
				float tentativeGScore = current->gScore + heuristic(current, neighbour);
				// Have we found a cheaper path to this neighbour?
				if (tentativeGScore < neighbour->gScore) {
					// Yes, yes we have.
					neighbour->parent = current;
					neighbour->gScore = tentativeGScore;
					neighbour->fScore = tentativeGScore + heuristic(neighbour, endNode);
					if (!neighbour->hasBeenConsidered) {
						openSet.push(neighbour);
						neighbour->hasBeenConsidered = true;
					}
				}
			}
		}

	}

	// No path found :(
	return std::vector<Node*>{};

}

void Pathfinder::run() {

	while (window.isOpen()) {

		window.clear();
		drawNodes();
		window.display();

		sf::Event event;
		if (window.waitEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::MouseMoved) {
				// Get screen position of currently hovered over Node
				int currX = event.mouseMove.x / tileWidth;
				int currY = event.mouseMove.y / tileHeight;
				// Check that the hovered tile is different to the previously hovered node
				if (highlightedTile.x != currX || highlightedTile.y != currY) {
					// Update which tile is being hovered
					highlightedTile.x = currX;
					highlightedTile.y = currY;
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				// Get the clicked on tile
				int x = event.mouseButton.x / tileHeight;
				int y = event.mouseButton.y / tileWidth;
				Node* currNode = getNode(x, y);
				if (currNode->tile != WALL && currNode != startNode) {
					endNode = currNode;
					resetNodes();
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

}

Pathfinder::Pathfinder() {
	// Don't change width, height, tileWidth because
	// hardcoded tilemap does not support any other size combination.
	width = 800;
	height = 600;
	tileWidth = 40;
	tileHeight = tileWidth;
	hTiles = width / tileWidth; // 20 horisontal tiles
	vTiles = height / tileHeight; // 15 vertical tiles
	loadTileMap();
	window.create(sf::VideoMode(width, height), "Pathfinder", sf::Style::Titlebar | sf::Style::Close);
	rectangleSize = sf::Vector2f((float)tileWidth - 2, (float)tileHeight - 2);
	startNode = nodes[1 * (width / tileWidth) + 1];
	highlightedTile.x = -1;
	highlightedTile.y = -1;
}