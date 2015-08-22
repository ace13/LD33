#pragma once

#include <Kunlaboro/Component.hpp>
#include <SFML/System/Vector2.hpp>

namespace Game
{

struct Physical : public Kunlaboro::Component
{
	sf::Vector2f Position, Velocity;
	float Radius;
	bool Blocking;

	Physical();
	void addedToEntity();
};

}

