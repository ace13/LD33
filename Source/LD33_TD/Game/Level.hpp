#pragma once

#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf { class RenderTarget; class VertexArray; }

class Level : public Kunlaboro::Component
{
public:
	enum Tile
	{
		Tile_None,
		Tile_Water,
		Tile_Grass
	};

	Level();

	void addedToEntity();
	

	void draw(sf::RenderTarget& target);

private:
	void drawTile(int x, int y, Tile tile, sf::VertexArray& arr);

	std::vector<Tile> mTiles;
	sf::Texture mTilesTexture;
};
