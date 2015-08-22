#include "Enemy.hpp"
#include "Components.hpp"
#include <Base/VectorMath.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

Enemy::Enemy() : Kunlaboro::Component("Game.Enemy"),
	mStrength(0), mHealth(1)
{

}

void Enemy::addedToEntity()
{
	requestMessage("LD33.Tick", [this](const Kunlaboro::Message& msg) { tick(msg.payload.get<float>()); });
	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg) { draw(*msg.payload.get<sf::RenderTarget*>()); });

	changeRequestPriority("LD33.Draw", 1);

	requestMessage("Level.PathRebuilt", [this](const Kunlaboro::Message& msg) {
		mPath = msg.payload.get<Path*>();
		mPathIter = mPath->begin();
	});

	requireComponent("Game.Physical", [this](const Kunlaboro::Message& msg) {
		mPhysical = (Game::Physical*)msg.sender;
		mPhysical->Radius = 8.f;

		auto resp = sendGlobalQuestion("Level.GetPath");
		if (resp.handled)
		{
			mPath = resp.payload.get<Path*>();
			mPathIter = mPath->begin();

			resp = sendGlobalQuestion("Level.HexToCoords", *mPathIter);
			mPhysical->Position = resp.payload.get<sf::Vector2f>();
		}
	});
}

void Enemy::damage(float dmg)
{
	mHealth -= dmg;

	if (mHealth <= 0)
		getEntitySystem()->destroyEntity(getOwnerId());
}

void Enemy::tick(float dt)
{
	if (mPathIter == mPath->end())
	{
		getEntitySystem()->destroyEntity(getOwnerId());
		return;
	}

	auto resp = sendGlobalQuestion("Level.HexToCoords", *mPathIter);
	mPhysical->Velocity = VMath::GetNormalized(resp.payload.get<sf::Vector2f>() - mPhysical->Position) * 64.f;

	if (VMath::Distance(mPhysical->Position, resp.payload.get<sf::Vector2f>()) < 8 * 8)
	{
		mPathIter++;
	}
}

void Enemy::draw(sf::RenderTarget& target)
{
	sf::CircleShape circ(mPhysical->Radius);
	circ.setFillColor(sf::Color::Red);

	circ.setOrigin(mPhysical->Radius, mPhysical->Radius);
	circ.setPosition(mPhysical->Position);

	target.draw(circ);
}

