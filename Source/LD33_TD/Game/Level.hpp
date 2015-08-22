#pragma once

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
	

	void draw(sf::RenderTarget& target);

private:
	void drawTile(int x, int y, Tile tile, sf::VertexArray& arr);

	int pickedX, pickedY;
	std::vector<Tile> mTiles;
	sf::Texture mTilesTexture;
};
