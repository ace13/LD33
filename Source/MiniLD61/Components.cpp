#include "Components.hpp"

#include "Game/Components.hpp"
#include "Game/Player.hpp"
#include "State/GameState.hpp"
#include "State/IntroState.hpp"
#include "State/MenuState.hpp"

#include <Kunlaboro/EntitySystem.hpp>

void Kunlaboro::RegisterMiniLD61(Kunlaboro::EntitySystem& sys)
{
	sys.registerComponent<Physical>("Game.Physical");
	sys.registerComponent<Player>("Game.Player");
	sys.registerComponent<GameState>("States.Game");


}
