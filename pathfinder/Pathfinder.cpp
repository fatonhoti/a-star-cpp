#include "Pathfinder.h"
#include <iostream>
#include <map>
#include <set>

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
		{ 'R', sf::Color::Green }}.at(tile);
}

Node Pathfinder::getNode(int x, int y) {
	return nodes[y * (width / tileWidth) + x];
}

void Pathfinder::drawTile(int x, int y, char tile) {
	sf::RectangleShape rect(sf::Vector2f((float)tileWidth - 2, (float)tileHeight - 2));
	rect.setPosition(x * tileWidth, y * tileHeight);
	rect.setFillColor(getColor(tile));
	if (tile != '#' && std::get<0>(highlightedTile) == x && std::get<1>(highlightedTile) == y) {
		// Highlight tile!
		rect.setOutlineColor(sf::Color::Red);
		rect.setOutlineThickness(2);
	}
	else if (std::get<0>(deHighlightedTile) == x && std::get<1>(deHighlightedTile) == y) {
		// Dehighlight tile!
		rect.setOutlineThickness(0);
	}
	window.draw(rect);
}

float Pathfinder::heuristic(int nodeX, int nodeY) {
	int goalX = width / tileWidth;
	int goalY = height / tileHeight;
	return std::sqrt(std::pow(nodeX - goalX, 2) + std::pow(nodeY - goalY, 2));
}

void Pathfinder::astar(int endX, int endY) {


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

			else if (event.type == sf::Event::MouseButtonReleased) {
				// Get clicked tile
				int x = event.mouseMove.x / tileWidth;
				int y = event.mouseMove.y / tileHeight;
				// Make sure it isn't a wall
				Node tile = nodes[y * (width / tileWidth) + x];
				if (tile.tile != '#') {
					// Start A*
					//astar(x, y);
					// Move robot
				}
			}
		}

		window.clear();
		for (int y = 0; y < height / tileHeight; y++) {
			for (int x = 0; x < width / tileWidth; x++) {
				char tile = getNode(x, y).tile;
				drawTile(x, y, tile);
			}
		}
		window.display();

	}

}

Pathfinder::Pathfinder() {
	width = 800;
	height = 600;
	tileWidth = 40;
	tileHeight = 40;
	title = "Pathfinder";
	loadMap();
	window.create(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close);
	deHighlightedTile = std::make_tuple(-1, -1);
	highlightedTile = std::make_tuple(-1, -1);
}