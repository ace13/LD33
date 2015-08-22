#include "Components.hpp"

#include <Base/Engine.hpp>
#include <Base/Music.hpp>
#include <Kunlaboro/EntitySystem.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

const float Engine::TICK_RATE = 1.f/20.f;

int main(int argc, char** argv)
{
	Engine eng;
	Kunlaboro::EntitySystem sys;
	sf::RenderWindow win;

	auto& mus = Engine::get<MusicManager>();
	{
		sf::SoundBuffer buf;
		buf.loadFromFile("Resources/bass.wav");

		mus.addTrack(0, std::move(buf));
	}

	mus[0].setVolume(25.f, 4);

	RegisterComponents(sys);

	eng.setSystem(sys);
	eng.setWindow(win);

	win.create(sf::VideoMode(800, 600), "Ludum Dare #33");
#ifndef NDEBUG
	win.setFramerateLimit(250);
#endif

	auto eid = sys.createEntity();
	sys.addComponent(eid, "Game.GameScreen");
	sys.addComponent(eid, "Game.Level");
	sys.sendLocalMessage(eid, "LoadLevel", std::string("Resources/Level1.txt"));

	eng.run();

	return 0;
}