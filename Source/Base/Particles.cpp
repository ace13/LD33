#include "Particles.hpp"
#include "Engine.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <random>
#include <cmath>

const float ParticleManager::ANGLE_RANDOM = -8008.135f;

ParticleManager::ParticleManager() :
	mNextLayer(Particle::Bottommost)
{

}
ParticleManager::ParticleManager(ParticleManager&& other) :
	mTexture(std::move(other.mTexture)), mNextLayer(std::move(other.mNextLayer)),
	mParticles(std::move(other.mParticles))
{
}

ParticleManager::~ParticleManager()
{

}

void ParticleManager::setTexture(sf::Texture&& tex)
{
	mTexture = std::move(tex);
}

void ParticleManager::addParticle(const Particle& p, const sf::Vector2f& position)
{
	InternalParticle ip{ p };

	ip.Position = position;
	if (ip.Angle == ANGLE_RANDOM)
	{
		std::random_device dev;
		std::uniform_real_distribution<float> rot(float(-M_PI), float(M_PI));

		ip.Angle = rot(dev);
	}

	for (auto& m : mParticles)
		if (m->mLayer == ip.Layer)
			m->mParticles.push_back(std::move(ip));
}
void ParticleManager::clearParticles()
{
	for (auto& m : mParticles)
		m->mParticles.clear();
}

ParticleManager::InternalParticle::InternalParticle(const Particle& p) :
	Life(0), Scale(p.StartScale), Color(p.StartColor)
{
	(Particle&)*this = p;
}

ParticleManager::InternalManager::InternalManager() : Kunlaboro::Component("LD33.Engine.ParticleManager")
{

}

void ParticleManager::InternalManager::addedToEntity()
{
	auto& man = Engine::get<ParticleManager>();

	mTexture = &man.mTexture;
	mLayer = man.mNextLayer;
	switch (mLayer)
	{
	case ParticleManager::Particle::Bottommost:
		man.mNextLayer = ParticleManager::Particle::Default; break;
	case ParticleManager::Particle::Default:
		man.mNextLayer = ParticleManager::Particle::Topmost; break;
	default:
		break;
	}

	requestMessage("LD33.Update", [this](const Kunlaboro::Message& msg) {
		update(msg.payload.get<float>());
	});
	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg) {
		draw(*msg.payload.get<sf::RenderTarget*>());
	});
	changeRequestPriority("LD33.Draw", int(mLayer));

	man.mParticles.push_back(this);
}

void ParticleManager::InternalManager::update(float dt)
{
	for (auto it = mParticles.begin(); it != mParticles.end();)
	{
		auto& p = *it;

		p.Life += dt;

		float perc = p.Life / p.Duration;
		p.Color = {
			sf::Uint8(p.StartColor.r + perc * (p.EndColor.r - p.StartColor.r)),
			sf::Uint8(p.StartColor.g + perc * (p.EndColor.g - p.StartColor.g)),
			sf::Uint8(p.StartColor.b + perc * (p.EndColor.b - p.StartColor.b)),
			sf::Uint8(p.StartColor.a + perc * (p.EndColor.a - p.StartColor.a))
		};

		p.Velocity += p.Gravity * dt;
		p.Position += p.Velocity * dt;
		p.Angle    += p.Rotation * dt;
		p.Scale     = p.StartScale + perc * (p.EndScale - p.StartScale);

		if (p.Life >= p.Duration)
			it = mParticles.erase(it);
		else
			++it;
	}
}

void ParticleManager::InternalManager::draw(sf::RenderTarget& target)
{
	sf::VertexArray vex(sf::Quads, mParticles.size() * 4);

	for (auto& p : mParticles)
	{
		sf::Vector2f x{
			std::cos(p.Angle) * p.Scale, std::sin(p.Angle) * p.Scale
		}, y{
			std::cos(p.Angle + float(M_PI_2)) * p.Scale, std::sin(p.Angle + float(M_PI_2)) * p.Scale
		};

		vex.append({
			p.Position - (p.Rect.width * x * 0.5f) - (p.Rect.height * y * 0.5f),
			p.Color,
			{ p.Rect.left, p.Rect.top } });
		vex.append({
			p.Position + (p.Rect.width * x * 0.5f) - (p.Rect.height * y * 0.5f),
			p.Color,
			{ p.Rect.left + p.Rect.width, p.Rect.top } });
		vex.append({
			p.Position + (p.Rect.width * x * 0.5f) + (p.Rect.height * y * 0.5f),
			p.Color,
			{ p.Rect.left + p.Rect.width, p.Rect.top + p.Rect.height } });
		vex.append({
			p.Position - (p.Rect.width * x * 0.5f) + (p.Rect.height * y * 0.5f),
			p.Color,
			{ p.Rect.left, p.Rect.top + p.Rect.height } });
	}

	sf::RenderStates states{ mTexture };
	target.draw(vex, states);
}
