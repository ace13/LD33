#include "Components.hpp"

#include "Game/Level.hpp"

#include <Kunlaboro/EntitySystem.hpp>

void RegisterComponents(Kunlaboro::EntitySystem& sys)
{
	sys.registerComponent<Level>("Game.Level");
}