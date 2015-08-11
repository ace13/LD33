#include "Components.hpp"

#include <Base/Engine.hpp>
#include <Kunlaboro/EntitySystem.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

int main(int argc, char** argv)
{
	bool fullscreen = false;
	bool borderless = false;

	for (int i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);

		if (arg == "-f" || arg == "--fullscreen")
			fullscreen = true;
		else if (arg == "-b" || arg == "--borderless")
			borderless = true;
	}

	sf::RenderWindow window;
	Kunlaboro::EntitySystem sys;

	Engine eng;
	eng.setSystem(sys);
	eng.setWindow(window);

	sf::Texture particles;
	particles.loadFromFile("Resources/Particles.png");
	Engine::get<ParticleManager>().setTexture(std::move(particles));

	// Window setup
	if (fullscreen)
		window.create(sf::VideoMode::getDesktopMode(), "MiniLD61", borderless ? sf::Style::None : sf::Style::Fullscreen);
	else
		window.create({ 800, 600 }, "MiniLD61");
	window.setFramerateLimit(300);

	// Entity setup
	Kunlaboro::RegisterMiniLD61(sys);
	sys.registerTemplate("Game.Player", { "Game.Player", "Game.Physical" });
	sys.registerTemplate("State.Intro", { "States.Intro" });
	sys.registerTemplate("State.Game", { "States.Game" });
	sys.registerTemplate("State.Menu", { "States.Menu" });

	// Starting state
	sys.createEntity("State.Game");

	eng.run();

	return 0;
}

