#include "StatDisplay.hpp"

StatDisplay::StatDisplay() : Kunlaboro::Component("Game.StatDisplay"),
	mSizeEaser(Tween::BounceOut)
{

}

void StatDisplay::addedToEntity()
{
	requireComponent("Game.Physical", [this](const Kunlaboro::Message& msg) {

	});

	requestMessage("LD33.Update", [this](const Kunlaboro::Message& msg) { update(msg.payload.get<float>()); });
	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg) { draw(*msg.payload.get<sf::RenderTarget*>()); });

	requestMessage("CloseDisplay", [this](Kunlaboro::Message& msg) {

	}, true);
}

void StatDisplay::update(float dt)
{
	mSizeEaser.update(dt);
}

void StatDisplay::draw(sf::RenderTarget& target)
{

}
