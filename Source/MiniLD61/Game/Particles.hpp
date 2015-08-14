#pragma once

#include <Base/Particles.hpp>

namespace Particles
{
	const ParticleManager::Particle FirePuff{
		ParticleManager::Particle::Default,
		0.5f, 0.05f, 0.25f,
		ParticleManager::ANGLE_RANDOM, 0.1f,
		{ 0, -1000 }, { 0, 1500 },
		{ 255, 200, 25 },
		{ 120, 5, 0 },
		{ 100, 100, 200, 200 }
	};
	const ParticleManager::Particle FireSmoke{
		ParticleManager::Particle::Default,
		1.f, 0.005f, 0.5f,
		ParticleManager::ANGLE_RANDOM, 0.1f,
		{ 0, -1000 }, { 0, 1500 },
		{ 179, 0, 0, 179 },
		{ 125, 125, 125, 15 },
		{ 100, 100, 200, 200 }
	};
}

