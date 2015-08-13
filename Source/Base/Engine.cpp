#include "Engine.hpp"
#include "Fonts.hpp"
#include "Input.hpp"
#include "Music.hpp"
#include "Particles.hpp"
#include "Profiling.hpp"

#include <Kunlaboro/EntitySystem.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include <chrono>
#include <sstream>
#include <stdexcept>

namespace {
	Engine* sEngine = nullptr;

	InputManager* sInput = nullptr;
	MusicManager* sMusic = nullptr;
	ParticleManager* sParticles = nullptr;
}

Engine::Engine() :
	mWindow(nullptr)
{
	sEngine = this;

	sInput = new InputManager;
	sMusic = new MusicManager;
	sParticles = new ParticleManager;
}
Engine::Engine(Engine&& other) :
	mWindow(std::move(other.mWindow)), mSystem(std::move(other.mSystem))
{
	sEngine = this;
}

Engine::~Engine()
{
	if (sEngine == this)
	{
		delete sInput;
		delete sMusic;
		delete sParticles;
	}
}

void Engine::setSystem(Kunlaboro::EntitySystem& sys)
{
	mSystem = &sys;

	sys.registerComponent<ParticleManager::InternalManager>("LD33.Engine.ParticleManager");
}
void Engine::setWindow(sf::RenderWindow& window)
{
	mWindow = &window;
}

void Engine::run()
{
	if (!mWindow || !mSystem)
		throw std::runtime_error("Missing window or entity system");

	auto eid = mSystem->createEntity();
	for (int i = 0; i < ParticleManager::NumLayers; ++i)
		mSystem->addComponent(eid, "LD33.Engine.ParticleManager");

	auto frame = std::chrono::high_resolution_clock::now(),
	     lastFrame = frame;
	auto time = lastFrame - frame, dt = time;
	float dtFloat;
	const auto tickRate =
		std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
			std::chrono::duration<float>(TICK_RATE)
		);

	sf::Event ev;
	sf::Text profilingText("", sf::getDefaultFont(), 10U);
	sf::View uiView = mWindow->getDefaultView(), gameView({}, { 0, 2500 });
	{
        sf::Vector2f size = (sf::Vector2f)mWindow->getSize();
        uiView.setSize(size);
        uiView.setCenter(size / 2.f);

        gameView.setSize(gameView.getSize().y * (size.x / size.y), gameView.getSize().y);
    }

	// Prime the profiler
	{ PROFILE_BLOCK("Frame");
		{ PROFILE_BLOCK("Events"); }
		{ PROFILE_BLOCK("Tick"); }
		{ PROFILE_BLOCK("Update"); }
		{ PROFILE_BLOCK("Draw");
			{ PROFILE_BLOCK("Game"); }
			{ PROFILE_BLOCK("UI"); }
		}
	} Profiler::resetBlocks();
    auto root = Profiler::getRoot()->getChild("Frame");

	while (mWindow->isOpen())
	{
		Profiler::startBlock("Frame");

		lastFrame = frame;
		frame = std::chrono::high_resolution_clock::now();

		dt = frame - lastFrame;
		dtFloat = std::chrono::duration_cast<std::chrono::duration<float>>(dt).count();
		time += dt;

		while (mWindow->pollEvent(ev))
		{ PROFILE_BLOCK("Events");
			switch(ev.type)
			{
			case sf::Event::Closed:
				mWindow->close(); break;
			case sf::Event::Resized:
			{
        		sf::Vector2f size = (sf::Vector2f)mWindow->getSize();
        		uiView.setSize(size);
        		uiView.setCenter(size / 2.f);

        		gameView.setSize(gameView.getSize().y * (size.x / size.y), gameView.getSize().y);
    		} break;

			default:
				sInput->handleEvent(ev);
				mSystem->sendGlobalMessage("LD33.Event", &ev);
				break;
			}
		}

		while (time > tickRate)
		{ PROFILE_BLOCK("Tick");
			sMusic->update(TICK_RATE);

			mSystem->sendGlobalMessage("LD33.Tick", TICK_RATE);
			time -= tickRate;
		}

		{ PROFILE_BLOCK("Update");
			mSystem->sendGlobalMessage("LD33.Update", dtFloat);
		}

		{ PROFILE_BLOCK("Draw");
			mWindow->clear();

			{ PROFILE_BLOCK("Game");
				mWindow->setView(gameView);
				mSystem->sendGlobalMessage("LD33.Draw", (sf::RenderTarget*)mWindow);
				gameView = mWindow->getView();
			}

			{ PROFILE_BLOCK("UI");
				mWindow->setView(uiView);
				mSystem->sendGlobalMessage("LD33.DrawUI", (sf::RenderTarget*)mWindow);
				mWindow->draw(profilingText);
			}

			PROFILE_CALL("Display", mWindow->display());
		}

		Profiler::endBlock(); // Frame

		if (root->getTotalTime() >= std::chrono::seconds(1))
        {
            std::ostringstream oss;
            oss << *root << std::endl;
            profilingText.setString(oss.str());

            Profiler::resetBlocks();
        }
	}
}

void Engine::close()
{
	mWindow->close();
}

template<>
InputManager& Engine::get()
{
	return *sInput;
}

template<>
MusicManager& Engine::get()
{
	return *sMusic;
}

template<>
ParticleManager& Engine::get()
{
	return *sParticles;
}

template<>
Engine& Engine::get()
{
	return *sEngine;
}

template<>
sf::Window& Engine::get() { return *sEngine->mWindow; }
template<>
sf::RenderWindow& Engine::get() { return *sEngine->mWindow; }
template<>
sf::RenderTarget& Engine::get() { return *sEngine->mWindow; }
template<>
Kunlaboro::EntitySystem& Engine::get() { return *sEngine->mSystem; }

