#include "GameScreen.hpp"
#include "Components.hpp"
#include "Wave.hpp"
#include "Towers/Basic.hpp"
#include "UI/TowerRadial.hpp"

#include <Base/Fonts.hpp>
#include <Base/Profiling.hpp>
#include <Base/Tweening.hpp>
#include <Base/VectorMath.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>


/// \FIXME TOO MUCH GLOBAL, VOLVO PLZ NERF
RadialMenu asdf;
int goldValue = 100, life = 100;
Profiler::TimePoint mouseDown;
Easer waveEaser(Tween::QuadraticIn);
WaveManager* wMan;


GameScreen::GameScreen() : Kunlaboro::Component("Game.GameScreen"),
	mMouseDown(false), mMenu(false), mSelectedTower(0), mTarget(nullptr)
{
	waveEaser.start(0, 1, 1);
	waveEaser.update(1);
}
void GameScreen::addedToEntity()
{
	requestComponent("Game.WaveManager", [](const Kunlaboro::Message& msg) {
		wMan = (WaveManager*)msg.sender;
	});

	requestMessage("GotGold", [](const Kunlaboro::Message& msg) {
		goldValue += msg.payload.get<int>();
	});

	requestMessage("SetCamera", [this](const Kunlaboro::Message& msg) {
		mCamera.setCenter(msg.payload.get<sf::Vector2f>());
	});

	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg) { draw(*msg.payload.get<sf::RenderTarget*>()); });
	requestMessage("LD33.DrawUI", [this](const Kunlaboro::Message& msg) { drawUI(*msg.payload.get<sf::RenderTarget*>()); });
	changeRequestPriority("LD33.Draw", -9001);
}
void GameScreen::event(sf::Event& ev)
{
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
					asdf.addEntry("Begin Wave", "Resources/Start.png");
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
	waveEaser.update(dt);
	asdf.update(dt);

	if (asdf.isClosed() && mMenu)
	{
		mMenu = false;

		if (asdf.getSelection() == "Begin Wave")
		{
			sendMessage("NextWave");
			waveEaser.reset();
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

	target.setView(mCamera);

	if (!asdf.isClosed())
	{

	}
}

void GameScreen::drawUI(sf::RenderTarget& target)
{
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
	gold.setString(std::to_string(life));


	target.draw(gold);

	gold.setString(wMan->getCurWave().Name);
	gold.setColor(sf::Color::White);

	gold.setPosition(10, target.getView().getSize().y - gold.getLocalBounds().height * 4);
	target.draw(gold);
}