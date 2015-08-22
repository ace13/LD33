#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace VMath
{
	inline float Dot(const sf::Vector2f& a, const sf::Vector2f& b) { return a.x * a.x + a.y * a.y; }
	inline float Distance(const sf::Vector2f& a, const sf::Vector2f& b) { sf::Vector2f diff{ a.x - b.x, a.y - b.y }; return Dot(diff, diff); }
	inline float DistanceSqrt(const sf::Vector2f& a, const sf::Vector2f& b) { sf::Vector2f diff{ a.x - b.x, a.y - b.y }; return std::sqrt(Dot(diff, diff)); }
}