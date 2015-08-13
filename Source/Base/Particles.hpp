#pragma once
#define _USE_MATH_DEFINES

#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <list>

namespace sf { class RenderTarget; }

class Engine;

class ParticleManager
{
public:
	static const float ANGLE_RANDOM;

	~ParticleManager();

	/// Particle data
	struct Particle
	{
		enum Priority : char
		{
			Bottommost = -127,
			Default = 0,
			Topmost = 127
		} Layer;
		float Duration,   ///< Lifetime in seconds
		      StartScale, ///< Scale at start
		      EndScale,   ///< Scale at end
			  Angle,      ///< Angle of the particle
		      Rotation;   ///< Rotation speed in radians/second
		sf::Vector2f Velocity, ///< The starting velocity
		             Gravity;  ///< The gravity of the particle
		sf::Color StartColor,  ///< Starting color of the particle
		          EndColor;    ///< Ending color of the particle
		sf::FloatRect Rect; ///< Texture rect for the particle
	};

	void setTexture(sf::Texture&&);

	void addParticle(const Particle& p, const sf::Vector2f& position);
	void clearParticles();

private:
	struct InternalParticle : public Particle
	{
		InternalParticle(const Particle&);

		float Life, ///< Current lifetime of the particle
		      Scale; ///< Current scale of the particle
		sf::Color Color; ///< Current color of the particle
		sf::Vector2f Position; ///< Particle position
	};

	class InternalManager : public Kunlaboro::Component
	{
	public:
		InternalManager();
		void addedToEntity();

		void update(float dt);
		void draw(sf::RenderTarget& target);

	private:
		ParticleManager::Particle::Priority mLayer;
		sf::Texture* mTexture;
		std::list<InternalParticle> mParticles;

		friend class ParticleManager;
	};

	ParticleManager();
	ParticleManager(ParticleManager&&);

	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	sf::Texture mTexture;
	Particle::Priority mNextLayer;
	std::list<InternalManager*> mParticles;

	friend class Engine;
	friend class InternalManager;
};

namespace Particles
{
	const ParticleManager::Particle CloudPuff{
		ParticleManager::Particle::Topmost, 10.f, 2.f, 1.75f, ParticleManager::ANGLE_RANDOM, 0.1f,
		{ 0, 500 }, { 0, 0 },
		{ 128, 128, 128, 129 },
		{ 179, 179, 179, 129 },
		{ 100, 100, 200, 200 }
	};
}
