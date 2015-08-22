#include "Level.hpp"
#include <Base/Fonts.hpp>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Window/Mouse.hpp>

#include <sstream>

const size_t LevelWidth = 12;
const size_t LevelHeight = 12;

Level::Level() : Kunlaboro::Component("Game.Level"),
	pickedX(0), pickedY(0)
{
	mTilesTexture.loadFromFile("Resources/Tiles.png");

	mTiles = {
		Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water,
		Tile_Water, Tile_Water, Tile_Mountain, Tile_Mountain, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Water,
		Tile_Water, Tile_Water, Tile_Mountain, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Water,
		Tile_Water, Tile_Mountain, Tile_Mountain, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_Grass, Tile_Grass, Tile_Water,
		Tile_Water, Tile_Mountain, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_Grass, Tile_None, Tile_None, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Forest, Tile_Mountain, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Forest, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Grass, Tile_Grass, Tile_Forest, Tile_Forest, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Mountain, Tile_Grass, Tile_Forest, Tile_Water, Tile_Water,
		Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_None, Tile_None, Tile_None, Tile_Water, Tile_Water, Tile_Water, Tile_Water
	};
}

void Level::addedToEntity()
{
	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg) { draw(*msg.payload.get<sf::RenderTarget*>()); });
}

void Level::draw(sf::RenderTarget& target)
{
	auto mouse = sf::Mouse::getPosition((sf::RenderWindow&)target);

	auto view = target.getView();
	view.zoom(0.25);
	view.move(9 * 32, 7 * 32);
	target.setView(view);

	auto worldMouse = target.mapPixelToCoords(mouse);
	pickedX = (worldMouse.x -8) / 49;
	pickedY = (worldMouse.y -32 - (pickedX%2==0)*16) / 32;

	sf::VertexArray tiles(sf::Quads, mTiles.size());

	for (int y = 0; y < LevelHeight; ++y)
	{
		for (int x = 0; x < LevelWidth; x += 2)
		{
			drawTile(x + 1, y, mTiles[x + 1 + y * LevelWidth], tiles);
			drawTile(x, y, mTiles[x + y * LevelWidth], tiles);
		}
	}

	target.draw(tiles, &mTilesTexture);

	view.move(9*-32, 7*-32);
	view.zoom(4);
	target.setView(view);
}

inline void Level::drawTile(int x, int y, Tile tile, sf::VertexArray& tiles)
{
	if (tile == Tile_None)
		return;

	const float width = 64;
	const float height = 32;

	int tx = tile % 3;
	int ty = tile / 3;

	tiles.append({
		{ x * 49.f, y * 32.f + (x % 2 == 0) * 16 },
		sf::Color::White,
		{ tx * 64.f, ty * 96.f }
	});
	tiles.append({
		{ x * 49.f + width, y * 32.f + (x % 2 == 0) * 16 },
		sf::Color::White,
		{ tx * 64.f + 64, ty * 96.f }
	});
	tiles.append({
		{ x * 49.f + width, y * 32.f + (x % 2 == 0) * 16 + 96 },
		sf::Color::White,
		{ tx * 64.f + 64, ty * 96.f + 96 }
	});
	tiles.append({
		{ x * 49.f, y * 32.f + (x % 2 == 0) * 16 + 96 },
		sf::Color::White,
		{ tx * 64.f, ty * 96.f + 96 }
	});
	
	if (x == pickedX && y == pickedY)
	{
		tx = 0;
		ty = 0;

		tiles.append({
			{ x * 49.f, y * 32.f + (x % 2 == 0) * 16 },
			sf::Color::White,
			{ tx * 64.f, ty * 96.f }
		});
		tiles.append({
			{ x * 49.f + width, y * 32.f + (x % 2 == 0) * 16 },
			sf::Color::White,
			{ tx * 64.f + 64, ty * 96.f }
		});
		tiles.append({
			{ x * 49.f + width, y * 32.f + (x % 2 == 0) * 16 + 96 },
			sf::Color::White,
			{ tx * 64.f + 64, ty * 96.f + 96 }
		});
		tiles.append({
			{ x * 49.f, y * 32.f + (x % 2 == 0) * 16 + 96 },
			sf::Color::White,
			{ tx * 64.f, ty * 96.f + 96 }
		});
	}
}
