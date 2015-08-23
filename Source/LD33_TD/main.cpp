#include "Components.hpp"
#include "Game/Level.hpp"

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
		buf.loadFromFile("Resources/bass.ogg");
		mus.addTrack(0, std::move(buf));

		buf.loadFromFile("Resources/bass2.ogg");
		mus.addTrack(1, std::move(buf));

		buf.loadFromFile("Resources/kicker.ogg");
		mus.addTrack(2, std::move(buf));

		buf.loadFromFile("Resources/loop.ogg");
		mus.addTrack(3, std::move(buf));

		buf.loadFromFile("Resources/test.ogg");
		mus.addTrack(4, std::move(buf));
	}

	//mus[0].setVolume(25.5f, 4);
	//mus[1].setVolume(50.f, 4);
	//mus[2].setVolume(10.f, 10);
	//mus[3].setVolume(50.f, 5);
	//mus[4].setVolume(75.f, 10);

	RegisterComponents(sys);

	eng.setSystem(sys);
	eng.setWindow(win);

	win.create(sf::VideoMode(800, 600), "Ludum Dare #33");

#ifndef NDEBUG
	win.setFramerateLimit(250);
#endif

	auto eid = sys.createEntity("Game.");

	Kunlaboro::Message msg;
	msg.payload = std::string("Resources/Level1.txt");
	sys.sendLocalMessage(eid, Kunlaboro::hash::hash_func1::hash("LoadLevel"), msg);

	if (msg.handled)
	{
		auto& level = *dynamic_cast<Level*>(msg.sender);

		auto size = sf::Vector2f(level.getSize()) * 24.f;
		sys.sendGlobalMessage("SetCamera", size);
	}

	eng.run();

	return 0;
}