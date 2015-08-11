#pragma once

#include <Kunlaboro/Component.hpp>

namespace sf { class RenderTarget; }

class GameState : public Kunlaboro::Component
{
public:
	GameState();
	~GameState() = default;

	void addedToEntity();

	void tick(float dt);
	void draw(sf::RenderTarget&);

	void addCloud();

private:
	float mCloudTimer;

};

