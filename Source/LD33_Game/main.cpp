#include <Base/Engine.hpp>
#include <Kunlaboro/EntitySystem.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

const float Engine::TICK_RATE = 1.f/20.f;

int main(int argc, char** argv)
{
	Engine eng;
	Kunlaboro::EntitySystem sys;
	sf::RenderWindow win;

	eng.setSystem(sys);
	eng.setWindow(win);

	win.create(sf::VideoMode(800, 600), "Ludum Dare #33");
#ifndef NDEBUG
	win.setFramerateLimit(250);
#endif

	eng.run();

	return 0;
}