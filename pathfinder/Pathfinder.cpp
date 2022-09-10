#include "Pathfinder.h"
#include <iostream>

std::string Pathfinder::loadMap() {
	// TODO: Load map from .txt file in the future
	std::string tiles = "";
	tiles += "####################";
	tiles += "#..................#";
	tiles += "####...............#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "#..................#";
	tiles += "####################";
	return tiles;
}

char Pathfinder::getTile(int x, int y) {
	//std::cout << y << ", " << x << "\n";
	//std::cout << y * tileWidth + x << "\n---------\n";
	return tiles[y * (width / tileWidth) + x];
}

void Pathfinder::setTile(int x, int y, char tile) {
	tiles[y * (width / tileWidth) + x] = tile;
}

void Pathfinder::drawTile(int x, int y, char tile) {
	sf::RectangleShape rect(sf::Vector2f((float)tileWidth, (float)tileHeight));
	rect.setPosition(x * tileWidth, y * tileHeight);
	if (tile == '#') {
		rect.setFillColor(sf::Color::Blue);
	}
	else if (tile == '.') {
		rect.setFillColor(sf::Color::White);
	}
	window.draw(rect);
}

void Pathfinder::run() {

	while (window.isOpen()) {
		sf::Event event;
		if (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		window.clear();

		// Draw tiles
		for (int y = 0; y < height / tileHeight; y++) {
			for (int x = 0; x < width / tileWidth; x++) {
				char tile = getTile(x, y);
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
	tiles = loadMap();
	window.create(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close);
}
