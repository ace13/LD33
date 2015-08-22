#pragma once

#include "Path.hpp"

#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf { class RenderTarget; class VertexArray; }

class Level : public Kunlaboro::Component
{
public:
	enum Tile
	{
		Tile_None = -1,

		Tile_Picker = 0,
		Tile_Water,
		Tile_Grass,
		Tile_Mountain,
		Tile_Forest,
	};

	Level();

	void addedToEntity();
	
	Path findPath(const sf::Vector2i& from, const sf::Vector2i& to) const;

	void draw(sf::RenderTarget& target);

	sf::Vector2f hexToCoords(const sf::Vector2i& hex) const;
	sf::Vector2i coordsToHex(const sf::Vector2f& coords) const;

private:
	void drawTile(const sf::Vector2i& pos, Tile tile, sf::VertexArray& arr);
	bool tilePassable(const sf::Vector2i& tile);

	int pickedX, pickedY;
	std::vector<Tile> mTiles;
	sf::Texture mTilesTexture;
	bool mRebuildPath;
	Path mBestPath;
};
