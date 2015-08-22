#include "Tower.hpp"
#include "Enemy.hpp"
#include "Components.hpp"
#include <Base/Profiling.hpp>
#include <Base/VectorMath.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

Tower::Tower() : Kunlaboro::Component("Game.Tower"),
	mDefinition(nullptr), mPhysical(nullptr), mTarget(nullptr), mXP(0), mCooldown(0), mLevel(0)
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
	if (!mTarget)
	{
		auto resp = sendGlobalQuestion("Game.Physical.Find", std::make_tuple(mPhysical->Position, 64.f));

		if (resp.handled)
		{
			mTarget = (Enemy*)getEntitySystem()->getAllComponentsOnEntity(resp.sender->getOwnerId(), "Game.Enemy").front();
		}
	}

	if (mTarget)
	{
		if (!getEntitySystem()->isValid(mTarget->getOwnerId()))
		{
			mTarget = nullptr;
			return;
		}

		auto resp = sendQuestionToEntity(mTarget->getOwnerId(), "GetPosition");
		if (!resp.handled || VMath::DistanceSqrt(mPhysical->Position, resp.payload.get<sf::Vector2f>()) > 64.f)
		{
			mTarget = nullptr;
			return;
		}

		mTarget->damage(dt / 2);
		if (!mTarget->isAlive())
		{
			mTarget = nullptr;
			return;
		}
	}
	else
		return;
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
