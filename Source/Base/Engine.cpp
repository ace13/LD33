#include "Engine.hpp"

#include <Kunlaboro/EntitySystem.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <chrono>
#include <stdexcept>

namespace {
	Engine* sEngine;
}


const float Engine::TICK_RATE = 1.f / 60.f;

Engine::Engine() :
	mWindow(nullptr)
{
	sEngine = this;
}
Engine::Engine(Engine&& other) :
	mMusic(std::move(other.mMusic)), mParticles(std::move(other.mParticles)),
	mWindow(std::move(other.mWindow))
{
}

Engine::~Engine()
{
}

void Engine::setSystem(Kunlaboro::EntitySystem& sys)
{
	mSystem = &sys;
}
void Engine::setWindow(sf::RenderWindow& window)
{
	mWindow = &window;
}

void Engine::run()
{
	if (!mWindow || !mSystem)
		throw std::runtime_error("Missing window or entity system");

	auto frame = std::chrono::high_resolution_clock::now(),
	     lastFrame = frame;
	auto time = lastFrame - frame, dt = time;
	float dtFloat;
	const auto tickRate =
		std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
			std::chrono::duration<float>(TICK_RATE)
		);

	sf::Event ev;

	while (mWindow->isOpen())
	{
		lastFrame = frame;
		frame = std::chrono::high_resolution_clock::now();

		dt = frame - lastFrame;
		dtFloat = std::chrono::duration_cast<std::chrono::duration<float>>(dt).count();
		time += dt;

		while (mWindow->pollEvent(ev))
		{
			switch(ev.type)
			{
				case sf::Event::Closed:
					mWindow->close(); break;

				default:
					mSystem->sendGlobalMessage("LD33.Event", ev);
					break;
			}
		}

		while (time >= tickRate)
		{
			mMusic.update(TICK_RATE);

			mSystem->sendGlobalMessage("LD33.Tick", TICK_RATE);
			time -= tickRate;
		}

		mParticles.update(dtFloat);
		mSystem->sendGlobalMessage("LD33.Update", dtFloat);

		mWindow->clear();

		mSystem->sendGlobalMessage("LD33.Draw", mWindow);
		mSystem->sendGlobalMessage("LD33.DrawUI", mWindow);

		mWindow->display();
	}
}

template<>
MusicManager& Engine::get()
{
	return sEngine->mMusic;
}

template<>
ParticleManager& Engine::get()
{
	return sEngine->mParticles;
}

