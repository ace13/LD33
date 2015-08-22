#include "Tower.hpp"
#include "Components.hpp"
#include <Base/Profiling.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

Tower::Tower() : Kunlaboro::Component("Game.Tower"),
	mDefinition(nullptr), mPhysical(nullptr), mXP(0), mCooldown(0), mLevel(0)
{

}

void Tower::addedToEntity()
{
	requireComponent("Game.Physical", [this](const Kunlaboro::Message& msg) {
		mPhysical = dynamic_cast<Game::Physical*>(msg.sender);

		mPhysical->Radius = 16.f;
		mPhysical->Blocking = true;
	});

	requestMessage("LD33.Tick", [this](const Kunlaboro::Message& msg) { tick(msg.payload.get<float>()); });
	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg) { draw(*msg.payload.get<sf::RenderTarget*>()); });

	changeRequestPriority("LD33.Draw", 1);
}

void Tower::tick(float dt)
{ PROFILE
	
}
void Tower::draw(sf::RenderTarget& target)
{
	sf::CircleShape circ(mPhysical->Radius);
	circ.setOrigin(mPhysical->Radius, mPhysical->Radius);
	circ.setPosition(mPhysical->Position);

	target.draw(circ);
}

void Tower::levelUp(float cost)
{
	mLevel++;

	if (cost < 0)
		mXP = 0;
	else
		mXP -= cost;

	sendMessage("LeveledUp");
}
