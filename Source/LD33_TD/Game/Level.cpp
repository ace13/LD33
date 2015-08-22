#include "Level.hpp"
#include <Base/Fonts.hpp>
#include <Base/Profiling.hpp>
#include <Base/VectorMath.hpp>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Window/Event.hpp>

#include <set>
#include <sstream>
#include <unordered_map>


namespace std
{
	template<>
	struct hash<sf::Vector2i>
	{
		size_t operator()(const sf::Vector2i& vec) const
		{
			return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
		}
	};
}

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
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Forest, Tile_Mountain, Tile_Grass, Tile_Grass, Tile_Forest, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Forest, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Forest, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Forest, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Grass, Tile_Grass, Tile_Forest, Tile_Forest, Tile_Water,
		Tile_Water, Tile_Grass, Tile_Grass, Tile_Grass, Tile_Grass, Tile_None, Tile_None, Tile_Mountain, Tile_Grass, Tile_Forest, Tile_Water, Tile_Water,
		Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_Water, Tile_None, Tile_None, Tile_None, Tile_Water, Tile_Water, Tile_Water, Tile_Water
	};
}

static Path toDraw;
static sf::Vector2i start;
static sf::RenderTarget* wind;
static sf::View mouseView;

void Level::addedToEntity()
{
	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg) { draw(*msg.payload.get<sf::RenderTarget*>()); });
	requestMessage("LD33.Event", [this](const Kunlaboro::Message& msg) {
		sf::Event& ev = *msg.payload.get<sf::Event*>();

		if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left)
		{
			auto worldMouse = wind->mapPixelToCoords(sf::Mouse::getPosition((sf::RenderWindow&)*wind), mouseView);
			start = { int(worldMouse.x - 8) / 49, int(worldMouse.y - 32 - (((int(worldMouse.x - 8) / 49) % 2) == 0) * 16) / 32 };
		}
		else if (ev.type == sf::Event::MouseButtonReleased && ev.mouseButton.button == sf::Mouse::Left)
		{
			auto worldMouse = wind->mapPixelToCoords(sf::Mouse::getPosition((sf::RenderWindow&)*wind), mouseView);
			auto end = sf::Vector2i{ int(worldMouse.x - 8) / 49, int(worldMouse.y - 32 - (((int(worldMouse.x - 8) / 49) % 2) == 0) * 16) / 32 };

			toDraw = findPath(start, end);
		}
	});

	toDraw = findPath({ 1, 10 }, { 8, 10 });
}

void Level::draw(sf::RenderTarget& target)
{ PROFILE
	wind = &target;
	auto mouse = sf::Mouse::getPosition((sf::RenderWindow&)target);

	auto view = target.getView();
	view.zoom(0.25);
	view.move(9 * 32, 7 * 32);
	target.setView(view);
	mouseView = view;

	auto worldMouse = target.mapPixelToCoords(mouse);
	pickedX = int(worldMouse.x -8) / 49;
	pickedY = int(worldMouse.y -32 - ((pickedX%2)==0)*16) / 32;

	sf::VertexArray tiles(sf::Quads, mTiles.size());

	for (int y = 0; y < LevelHeight; ++y)
	{
		for (int x = 0; x < LevelWidth; x += 2)
		{
			drawTile({ x + 1, y }, mTiles[x + 1 + y * LevelWidth], tiles);
			drawTile({ x, y }, mTiles[x + y * LevelWidth], tiles);
		}
	}

	for (auto& step : toDraw)
	{
		drawTile(step, Tile_Picker, tiles);
	}

	target.draw(tiles, &mTilesTexture);

	

	view.move(9*-32, 7*-32);
	view.zoom(4);
	target.setView(view);
}

inline void Level::drawTile(const sf::Vector2i& pos, Tile tile, sf::VertexArray& tiles)
{
	if (tile == Tile_None)
		return;

	const float width = 64;
	const float height = 32;

	int x = pos.x;
	int y = pos.y;
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

Path Level::findPath(const sf::Vector2i& from, const sf::Vector2i& to) const
{ PROFILE
	if (from == to)
		return Path::Invalid;

	sf::IntRect level{ 0, 0, int(LevelWidth), int(LevelHeight) };
	if (!level.contains(from) || !level.contains(to))
		return Path::Invalid;

	Path ret;

	auto heuristic = [&from](const sf::Vector2i& a, const sf::Vector2i& b) -> float
	{
		
		sf::Vector3f eA(a.x, -a.x - a.y, a.y - (a.x - (a.x & 1)) / 2.f),
			eB(b.x, -b.x - b.y, b.y - (b.x - (b.x & 1)) / 2.f);

		float dx1 = a.x - b.x,
			dy1 = a.y - b.y,
			dx2 = from.x - b.x,
			dy2 = from.y - b.y;
		float cross = std::abs(dx1*dy2 - dx2*dy1);

		return (std::abs(eA.x - eB.x) + std::abs(eA.y - eB.y) + std::abs(eA.z - eB.z)) / 2.f + cross * 0.001f;
		
		/*
		
		
		return (std::abs(a.x - b.x) + std::abs(a.y - b.y)) / 2.f;
		*/


		// Euclidian distance
		//return VMath::DistanceSqrt(sf::Vector2f(a), sf::Vector2f(b));
	};
	auto cost = [this](const sf::Vector2i& a) -> float
	{
		if (a.x < 0 || a.y < 0 || a.x >= LevelWidth || a.y >= LevelHeight)
			return -1;
		if (mTiles[a.x + a.y * LevelWidth] != Tile_Grass)
			return -1;
		return 1;
	};

	struct AStarNode
	{
		sf::Vector2i Parent;
		sf::Vector2i Pos;
		float Priority;

		inline bool operator==(const sf::Vector2i& rhs) const
		{
			return Pos == rhs;
		}
		inline bool operator!=(const sf::Vector2i& rhs) const
		{
			return Pos != rhs;
		}
		inline bool operator<(const AStarNode& rhs) const
		{
			return Priority < rhs.Priority;
		}
	};

	/// \TODO A*

	AStarNode start{ from, from, 0 };

	std::unordered_map<sf::Vector2i, AStarNode> openList;
	auto cheapest = [&openList]() -> AStarNode
	{
		AStarNode lowest = { {}, {}, FLT_MAX };
		for (auto& it : openList)
			if (it.second < lowest)
				lowest = it.second;

		return lowest;
	};

	std::unordered_map<sf::Vector2i, AStarNode> closedSet;
	
	openList.emplace(from, AStarNode{ from, from, 0 });
	while (!openList.empty() && cheapest() != to)
	{
		auto cur = cheapest();
		openList.erase(cur.Pos);

		closedSet[cur.Pos] = cur;
		
		sf::Vector2i neighbors[6] = {
			cur.Pos + sf::Vector2i(-1, 0),
			cur.Pos + sf::Vector2i(1, 0),
			cur.Pos + sf::Vector2i(0, -1),
			cur.Pos + sf::Vector2i(0, 1),

			cur.Pos + sf::Vector2i(-1, ((cur.Pos.x & 1) ? -1 : 1)),
			cur.Pos + sf::Vector2i(1, ((cur.Pos.x & 1) ? -1 : 1))
		};

		for (auto& n : neighbors)
		{
			if (cost(n) < 0)
				continue;

			float g = cur.Priority + heuristic(cur.Pos, n);
			
			if (openList.count(n) > 0 && openList[n].Priority > g)
				openList.erase(n);
			if (closedSet.count(n) > 0 && closedSet[n].Priority > g)
				closedSet.erase(n);

			if (closedSet.count(n) == 0 && openList.count(n) == 0)
			{
				AStarNode ne;
				ne.Pos = n;
				ne.Priority = g + heuristic(n, to);
				ne.Parent = cur.Pos;

				openList[n] = ne;
			}
		}
	}

	if (openList.empty())
		return Path::Invalid;

	// Follow Parent pointers back to start
	auto curPoint = cheapest();
	
	do
	{
		ret.push(curPoint.Pos);
		curPoint = closedSet[curPoint.Parent];
	} while (curPoint.Pos != from);
	ret.push(from);

	return ret;
}

