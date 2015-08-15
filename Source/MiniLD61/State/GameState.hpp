#pragma once

#include <Kunlaboro/Component.hpp>

<<<<<<< Updated upstream
namespace sf { struct Event; class RenderTarget; }
=======
namespace sf { class Event; class RenderTarget; }
>>>>>>> Stashed changes

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

