#include "Basic.hpp"
#include "../Particles.hpp"
#include "../Components.hpp"

#include <Base/Engine.hpp>

using namespace Towers;

const Basic Basic::Instance;
const Firehose Firehose::Instance;

void Basic::fire(const Game::Physical& source, Game::Physical& target) const
{
	auto& pm = Engine::get<ParticleManager>();

	for (int i = 0; i < 4; ++i)
	{
		auto p = Particles::Water;

		p.Velocity = ((target.Position + target.Velocity * p.Duration) - source.Position) / p.Duration + sf::Vector2f(rand()%10 - 5, rand()%10 - 5);

		pm.addParticle(p, source.Position);
	}

	target.Velocity *= 0.9f;
}

void Basic::draw(const Game::Physical& position, const Game::Physical* target, sf::RenderTarget& rt) const
{

}

void Firehose::fire(const Game::Physical& source, Game::Physical& target) const
{
	auto& pm = Engine::get<ParticleManager>();

	for (int i = 0; i < 8; ++i)
	{
		auto p = Particles::Water;

		p.Duration = 0.1;
		p.Velocity = ((target.Position + target.Velocity * p.Duration) - source.Position) / p.Duration + sf::Vector2f(rand() % 10 - 5, rand() % 10 - 5);

		pm.addParticle(p, source.Position);
	}

	target.Velocity *= 0.01f;
	//target.Position -= (source.Position - target.Position) * 0.075f;
}
void Firehose::draw(const Game::Physical& position, const Game::Physical* target, sf::RenderTarget& rt) const
{

}
