#include "Tower.hpp"
#include "Enemy.hpp"
#include "Components.hpp"
#include "Particles.hpp"

#include <Base/Engine.hpp>
#include <Base/Profiling.hpp>
#include <Base/VectorMath.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

Tower::Tower() : Kunlaboro::Component("Game.Tower"),
	mDefinition(nullptr), mPhysical(nullptr), mTarget(nullptr), mXP(0), mCooldown(0), mLevel(1)
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


	requestMessage("GotXP", [this](const Kunlaboro::Message& msg) {
		mXP += msg.payload.get<float>();
	}, true);
	requestMessage("Sold", [this](const Kunlaboro::Message& msg) {
		sendGlobalMessage("GotGold", int(mDefinition->getPrice() * 0.75f));

		auto& man = Engine::get<ParticleManager>();
		for (int i = 0; i < 8; ++i)
		{
			auto p = Particles::Gold;
			float ang = (rand() % 360) * 180.f / M_PI;

			p.Velocity = sf::Vector2f(std::cos(ang), std::sin(ang)) * 100.f;

			man.addParticle(p, mPhysical->Position);
		}
	}, true);
	requestMessage("Define", [this](const Kunlaboro::Message& msg) {
		mDefinition = msg.payload.get<const TowerDefinition*>();
	}, true);

	changeRequestPriority("LD33.Draw", 1);
}

void Tower::tick(float dt)
{ PROFILE;
	float nextLevel = 100 + mLevel * 100;
	if (mXP >= nextLevel)
		levelUp(nextLevel);

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

		mTarget->damage(dt / 2.f * (1 + (mLevel-1)/10.f));
		if (!mTarget->isAlive() && !mTarget->isDeathHandled())
		{
			mTarget->markDeathHandled();

			float totalXP = mTarget->getXP() / 5.f;
			mXP += totalXP * 2;

			/// \TODO: Assists
			//mXP += totalXP * 2;

			std::vector<Game::Physical*> blockers;
			auto aoe = sendGlobalQuestion("Game.Physical.Blocking", std::make_tuple(&blockers, resp.payload.get<sf::Vector2f>(), 128.f));
			for (auto& tower : blockers)
			{
				sendMessageToEntity(tower->getOwnerId(), "GotXP", (totalXP * 3) / blockers.size());
			}

			sendGlobalMessage("GotGold", mTarget->getGold());
			auto& man = Engine::get<ParticleManager>();
			for (int i = 0; i < 4; ++i)
			{
				auto p = Particles::Gold;
				float ang = (rand() % 360) * 180.f / M_PI;

				p.Velocity = sf::Vector2f(std::cos(ang), std::sin(ang)) * 100.f;

				man.addParticle(p, resp.payload.get<sf::Vector2f>());
			}

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

const TowerDefinition& Tower::getDefinition() const
{
	return *mDefinition;
}

void Tower::levelUp(float cost)
{
	mLevel++;

	if (cost < 0)
		mXP = 0;
	else
		mXP -= cost;

	sendMessage("LeveledUp");

	auto& man = Engine::get<ParticleManager>();
	for (int i = 0; i < 360; ++i)
	{
		auto p = Particles::LevelUp;
		float ang = i * 180.f / M_PI;

		sf::Vector2f dir(std::cos(ang), std::sin(ang));

		p.Angle = ang;
		p.Velocity = dir * 100.f;
		p.Gravity = dir * -100.f / p.Duration;

		man.addParticle(p, mPhysical->Position);
	}
}
