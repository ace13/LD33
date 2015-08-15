#pragma once

#include <Kunlaboro/Component.hpp>

namespace sf { class Event; class RenderTarget; }

class GameState : public Kunlaboro::Component
{
public:
	GameState();
	~GameState() = default;

	void addedToEntity();

	void event(sf::Event& ev);
	void tick(float dt);
	void draw(sf::RenderTarget& target);

	void addCloud();

private:
	float mCloudTimer;

};

