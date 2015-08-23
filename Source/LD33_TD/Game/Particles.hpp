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
}
