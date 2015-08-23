#include "Enemy.hpp"
#include "Components.hpp"
#include <Base/Profiling.hpp>
#include <Base/VectorMath.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>

Enemy::Enemy() : Kunlaboro::Component("Game.Enemy"),
	mStrength(1), mHealth(1), mDeathHandled(false)
{

}

void Enemy::addedToEntity()
{
	requestMessage("LD33.Tick", [this](const Kunlaboro::Message& msg) { tick(msg.payload.get<float>()); });
	changeRequestPriority("LD33.Tick", 9001); // Tick enemies last

	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg) { draw(*msg.payload.get<sf::RenderTarget*>()); });
	changeRequestPriority("LD33.Draw", 1);

	requestMessage("Level.PathRebuilt", [this](const Kunlaboro::Message& msg) {
		auto oldPos = *mPathIter;

		mPath = msg.payload.get<Path*>();
		mPathIter = mPath->begin();

		auto it = std::find(mPath->begin(), mPath->end(), oldPos);
		if (it != mPath->end())
			mPathIter = it;
		else
		{
			float shortest = FLT_MAX;
			auto it = mPath->begin();
			for (; it != mPath->end(); ++it)
			{
				auto resp = sendGlobalQuestion("Level.HexToCoords", *it);
				float dist = VMath::Distance(mPhysical->Position, resp.payload.get<sf::Vector2f>());

				if (dist < shortest)
				{
					shortest = dist;
					mPathIter = it;
				}
			}

			if (it != mPath->end())
				it++;
		}
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
	mHealth -= dmg / mStrength;
}

void Enemy::markDeathHandled()
{
	mDeathHandled = true;
}
bool Enemy::isDeathHandled() const
{
	return mDeathHandled;
}

bool Enemy::isAlive() const
{
	return mHealth > 0;
}
float Enemy::getXP() const
{
	return 10 + 10 * mStrength;
}
int Enemy::getGold() const
{
	return 10 + 10 * (mStrength - 1);
}

void Enemy::tick(float dt)
{ PROFILE;
	if (mHealth <= 0)
	{
		getEntitySystem()->destroyEntity(getOwnerId());
		return;
	}
	
	if (mPathIter == mPath->end())
	{
		sendGlobalMessage("Enemy.ReachedGoal");
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
	circ.setFillColor({ sf::Uint8(mHealth * 255), 0, 0 });

	circ.setOrigin(mPhysical->Radius, mPhysical->Radius);
	circ.setPosition(mPhysical->Position);

	target.draw(circ);
}

