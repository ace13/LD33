#include "GameScreen.hpp"
#include "Components.hpp"
#include "Enemy.hpp"
#include "Level.hpp"
#include "Wave.hpp"
#include "Towers/Basic.hpp"
#include "UI/TowerRadial.hpp"

#include <Base/Engine.hpp>
#include <Base/Fonts.hpp>
#include <Base/Music.hpp>
#include <Base/Profiling.hpp>
#include <Base/Tweening.hpp>
#include <Base/VectorMath.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include <iostream>


/// \FIXME TOO MUCH GLOBAL, VOLVO PLZ NERF
RadialMenu asdf;
int goldValue = 100;
float life = 100;
Profiler::TimePoint mouseDown;
Easer waveEaser(Tween::QuadraticIn), newspaperSizeEaser(Tween::QuarticOut), newspaperSpinEaser(Tween::QuadraticOut), newspaperAlphaEase(Tween::QuadraticIn);
WaveManager* wMan;
bool preLevel = false, wasWave = false;
size_t lastEnemies = 0, levelCount = 1;
Level* level;
sf::Texture newspaperTex;

GameScreen::GameScreen() : Kunlaboro::Component("Game.GameScreen"),
	mMouseDown(false), mMenu(false), mSelectedTower(0), mTarget(nullptr)
{
	waveEaser.start(0, 1, 1);
	waveEaser.update(1);
	newspaperTex.loadFromFile("Resources/Newspaper.png");
}
void GameScreen::addedToEntity()
{
	requestComponent("Game.WaveManager", [](const Kunlaboro::Message& msg) {
		wMan = (WaveManager*)msg.sender;
	});

	requestMessage("GotGold", [](const Kunlaboro::Message& msg) {
		goldValue += msg.payload.get<int>();
	});
	requestMessage("Enemy.ReachedGoal", [this](const Kunlaboro::Message& msg) {
		Enemy* send = (Enemy*)msg.sender;
		life -= send->getStrength();

		if (life <= 0)
		{
			std::cout << "TODO: Lose game" << std::endl;
			((sf::RenderWindow*)mTarget)->close();
		}
	});
	requestMessage("LevelLoaded", [this](const Kunlaboro::Message& msg){
		levelCount++;
		preLevel = true;

		level = (Level*)msg.sender;
		newspaperSizeEaser.start(0, 1, 4);
		newspaperSpinEaser.start((1500 + rand() % 360) * ((rand() % 2) == 0 ? -1 : 1), 10 + (rand()%7), 4);
		newspaperAlphaEase.start(0, 255, 6);

		auto& man = Engine::get<MusicManager>();
		man[3].setVolume(0);
		man[4].setVolume(0);

		man[3 + (rand() % 2)].setVolume(25, 2);
	},true);

	requestMessage("SetCamera", [this](const Kunlaboro::Message& msg) {
		mCamera.setCenter(msg.payload.get<sf::Vector2f>());
	});

	requestMessage("LD33.Draw", [this](Kunlaboro::Message& msg) {
		draw(*msg.payload.get<sf::RenderTarget*>());

		if (preLevel)
			msg.handle(nullptr);
	});
	requestMessage("LD33.DrawUI", [this](const Kunlaboro::Message& msg) { drawUI(*msg.payload.get<sf::RenderTarget*>()); });
	changeRequestPriority("LD33.Draw", -9001);
}
void GameScreen::event(sf::Event& ev)
{
	if (preLevel)
	{
		if (ev.type == sf::Event::KeyReleased || ev.type == sf::Event::MouseButtonReleased)
			preLevel = false;

		return;
	}

	if (!asdf.isClosed())
		asdf.event(ev);

	if (ev.type == sf::Event::MouseButtonPressed)
	{
		if (ev.mouseButton.button == sf::Mouse::Left && asdf.isClosed())
		{
			mouseDown = Profiler::Clock::now();
			mMouseDown = true;
			mMousePos = sf::Mouse::getPosition(*(sf::RenderWindow*)mTarget);
		}
	}
	else if (ev.type == sf::Event::MouseButtonReleased)
	{
		if (ev.mouseButton.button == sf::Mouse::Left && asdf.isClosed())
		{
			mMouseDown = false;
			auto now = Profiler::Clock::now();
			if ((now - mouseDown) < std::chrono::duration<int, std::milli>(150))
			{
				asdf.clearEntries();

				if (wMan->getCurWave() == WaveManager::Wave::None)
				{
					std::vector<Game::Physical*> test;
					sendGlobalMessage("Game.Physical.Find", &test);
					if (test.empty())
						asdf.addEntry("Begin Wave", "Resources/Start.png");
				}
				asdf.setPosition({ float(ev.mouseButton.x), float(ev.mouseButton.y) });

				auto resp = sendGlobalQuestion("Level.Valid", mTarget->mapPixelToCoords({ ev.mouseButton.x, ev.mouseButton.y }, mCamera));
				if (resp.handled && resp.payload.get<bool>())
				{
					mMenu = true;

					if (goldValue >= 25)
						asdf.addEntry("Water Pistol (25G)", "Resources/Hammer.png");
				}
				else if (resp.handled && !resp.payload.get<bool>())
				{
					auto mPos = mTarget->mapPixelToCoords({ ev.mouseButton.x, ev.mouseButton.y }, mCamera);
					std::vector<Game::Physical*> blockers;
					sendGlobalMessage("Game.Physical.Blocking", &blockers);
					for (auto& it : blockers)
					{
						if (VMath::Distance(it->Position, mPos) < it->Radius * it->Radius)
						{
							mMenu = true;
							mSelectedTower = it->getOwnerId();

							asdf.addEntry("Sell", "Resources/Coins.png");

							if (goldValue >= 75)
								asdf.addEntry("Firehose Upgrade (75G)", "Resources/Plus.png");
							asdf.addEntry("Do something else", "Resources/Plus.png");

							asdf.setPosition({ float(ev.mouseButton.x), float(ev.mouseButton.y) });
							break;
						}
					}
				}

				if (asdf.hasEntries())
					asdf.open();
			}
		}
	}
	else if (ev.type == sf::Event::MouseWheelMoved && asdf.isClosed())
	{
		auto delta = ev.mouseWheel.delta;

		if (delta > 0)
		{
			for (int i = 0; i < delta; ++i)
			{
				if (mCurZoom < 0.15)
					continue;

				mCamera.zoom(0.75f);
				mCurZoom *= 0.75f;
			}
		}
		else
			for (int i = 0; i > delta; --i)
			{
				if (mCurZoom > 0.5)
					continue;

				mCamera.zoom(1.25f);
				mCurZoom *= 1.25f;
			}
	}
}
void GameScreen::update(float dt)
{
	if (preLevel)
	{
		newspaperSizeEaser.update(dt);
		newspaperSpinEaser.update(dt);
		newspaperAlphaEase.update(dt);

		if (newspaperAlphaEase.atEnd())
			preLevel = false;
		return;
	}

	bool inWave = wMan->getCurWave() != WaveManager::Wave::None;
	if (!inWave && wasWave)
	{
		auto& man = Engine::get<MusicManager>();
		man[2].setVolume(0, 8);
		man[1].setVolume(0, 8);
		man[0].setVolume(0, 2);

		man[3 + (rand() % 2)].setVolume(25, 5);
	}
	wasWave = inWave;

	if (inWave)
	{
		std::vector<Game::Physical*> test;
		sendGlobalMessage("Game.Physical.Find", &test);

		auto& man = Engine::get<MusicManager>();
		if (test.size() > 15 && lastEnemies < 10)
		{
			man[0].setVolume(10, 2);
			lastEnemies = test.size();
		}
		else if (test.size() < 10 && lastEnemies > 15)
		{
			man[0].setVolume(0, 2);
			lastEnemies = test.size();
		}
		else if (test.empty())
		{

		}
	}

	waveEaser.update(dt);
	asdf.update(dt);

	if (asdf.isClosed() && mMenu)
	{
		mMenu = false;

		if (asdf.getSelection() == "Begin Wave")
		{
			if (wMan->getNextWave() == WaveManager::Wave::None)
			{
				std::vector<Game::Physical*> blockers;
				auto aoe = sendGlobalQuestion("Game.Physical.Blocking", &blockers);
				for (auto& tower : blockers)
					getEntitySystem()->destroyEntity(tower->getOwnerId());

				auto resp = sendQuestion("LoadLevel", "Resources/Level" + std::to_string(levelCount) + ".txt");


				if (!resp.payload.get<bool>())
				{
					std::cout << "TODO: End game screen" << std::endl;
					((sf::RenderWindow*)mTarget)->close();
				}
			}
			else
			{
				auto& man = Engine::get<MusicManager>();
				man[2].setVolume(75, 10);
				man[1].setVolume(25, 15);

				man[3].setVolume(0, 5);
				man[4].setVolume(0, 5);

				sendMessage("NextWave");
				waveEaser.reset();
			}
			return;
		}

		if (mSelectedTower != 0)
		{
			if (asdf.getSelection() == "Sell")
			{
				sendMessageToEntity(mSelectedTower, "Sold");
				getEntitySystem()->destroyEntity(mSelectedTower);
			}
			else if (asdf.getSelection().substr(0,8) == "Firehose")
			{
				goldValue -= 75;
				sendMessageToEntity(mSelectedTower, "Define", &Towers::Firehose::Instance);
			}

			mSelectedTower = 0;
		}
		else
		{
			if (asdf.getSelection().substr(0,12) == "Water Pistol")
			{
				goldValue -= 25;
				auto eid = getEntitySystem()->createEntity("Game.Tower");

				auto resp = sendGlobalQuestion("Level.CoordsToHex", mTarget->mapPixelToCoords(sf::Vector2i(asdf.getPosition()), mCamera));
				resp = sendGlobalQuestion("Level.HexToCoords", resp.payload.get<sf::Vector2i>());
				sendMessageToEntity(eid, "SetPosition", resp.payload.get<sf::Vector2f>());

				sendMessageToEntity(eid, "Define", &Towers::Basic::Instance);
			}
		}
	}

	if (mMouseDown)
	{
		auto mousePos = sf::Mouse::getPosition(*(sf::RenderWindow*)mTarget);
		auto diff = mousePos - mMousePos;

		mCamera.move(sf::Vector2f(-diff) * (mCurZoom * 4));
		mMousePos = mousePos;
	}
	
}
void GameScreen::draw(sf::RenderTarget& target)
{
	if (!mTarget)
	{
		mTarget = &target;
		requestMessage("LD33.Update", [this](const Kunlaboro::Message& msg) { update(msg.payload.get<float>()); });
		requestMessage("LD33.Event", [this](const Kunlaboro::Message& msg) { event(*msg.payload.get<sf::Event*>()); });

		auto old = mCamera;
		mCamera = target.getView();
		mCamera.zoom(0.25);
		mCamera.setCenter(old.getCenter());
		mCurZoom = 0.25;
	}
	
	if (preLevel)
	{
		return;
	}

	target.setView(mCamera);

	if (!asdf.isClosed())
	{

	}
}

void GameScreen::drawUI(sf::RenderTarget& target)
{
	if (preLevel)
	{
		sf::Sprite news(newspaperTex);
		news.setOrigin(sf::Vector2f(newspaperTex.getSize()) / 2.f - sf::Vector2f(10, 0));
		news.setPosition(target.getView().getCenter());
		news.setRotation(*newspaperSpinEaser);
		news.setScale(*newspaperSizeEaser, *newspaperSizeEaser);
		news.setColor(sf::Color(255, 255, 255, *newspaperAlphaEase));

		target.draw(news);

		sf::Vector2f y{
			std::cos((*newspaperSpinEaser - 90) * float(M_PI) / 180.f),
			std::sin((*newspaperSpinEaser - 90) * float(M_PI) / 180.f)
		};

		sf::Text title(level->getTitle(), sf::getDefaultFont(), 26);

		auto bounds = title.getLocalBounds();
		title.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
		title.setPosition(news.getPosition() + (y * 90.f));
		title.setStyle(sf::Text::Bold);
		title.setRotation(*newspaperSpinEaser);
		title.setScale(*newspaperSizeEaser, *newspaperSizeEaser);
		title.setColor(sf::Color(0, 0, 0, std::min(*newspaperAlphaEase * 2, 255.f)));

		target.draw(title);

		title.setString(level->getSubtitle());
		title.setCharacterSize(16);

		bounds = title.getLocalBounds();

		title.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
		title.setPosition(news.getPosition() + (y * 60.f));

		title.setColor(sf::Color(12, 12, 12, std::min(*newspaperAlphaEase * 2, 255.f)));

		target.draw(title);

		return;
	}

	asdf.draw(target);

	sf::Text gold("Gold: ", sf::getDefaultFont(), 20);
	gold.setPosition(10, target.getView().getSize().y - gold.getLocalBounds().height * 2);

	target.draw(gold);

	gold.move(gold.getLocalBounds().width, 0);
	gold.setString(std::to_string(goldValue));
	gold.setColor({ 255, 255, 0 });

	target.draw(gold);

	gold.move(gold.getLocalBounds().width + 24, 0);
	gold.setString("Lives: ");
	gold.setColor(sf::Color::White);
	target.draw(gold);

	gold.move(gold.getLocalBounds().width, 0);
	gold.setString(std::to_string(int(life)));


	target.draw(gold);

	gold.setString(wMan->getCurWave().Name);
	gold.setColor(sf::Color::White);

	gold.setPosition(10, target.getView().getSize().y - gold.getLocalBounds().height * 4);
	target.draw(gold);
}