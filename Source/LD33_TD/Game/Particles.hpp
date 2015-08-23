#pragma once

#include <Base/Particles.hpp>

namespace Particles
{
	const ParticleManager::Particle LevelUp{
		ParticleManager::Particle::Default, 1.5f, 2.f, 0.f, 0, 0,
		{ 0, 0 }, { 0, 0 },
		{ 255, 255, 190, 179 },
		{ 179, 179, 2, 1 },
		{ 0, 0, 2, 25 }
	};

	const ParticleManager::Particle Gold{
		ParticleManager::Particle::Default, .5f, 1.f, 1.5f, ParticleManager::ANGLE_RANDOM, 0.5f,
		{ 0, 0 }, { 0, 0 },
		{ 255, 255, 0, 179 },
		{ 179, 170, 0, 0 },
		{ 0, 100, 8, 8 }
	};
}
