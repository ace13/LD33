#include "Components.hpp"

#include "Game/Enemy.hpp"
#include "Game/Components.hpp"
#include "Game/Level.hpp"
#include "Game/Tower.hpp"

#include <Kunlaboro/EntitySystem.hpp>

void RegisterComponents(Kunlaboro::EntitySystem& sys)
{
	sys.registerComponent<Enemy>("Game.Enemy");
	sys.registerComponent<Level>("Game.Level");
	sys.registerComponent<Game::Physical>("Game.Physical");
	sys.registerComponent<Tower>("Game.Tower");

	sys.registerTemplate("Game.Enemy", { "Game.Enemy", "Game.Physical" });
	sys.registerTemplate("Game.Tower", { "Game.Tower", "Game.Physical" });
}