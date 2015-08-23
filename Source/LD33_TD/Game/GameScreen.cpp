#include "GameScreen.hpp"
#include "Components.hpp"
#include "Towers/Basic.hpp"
#include "UI/TowerRadial.hpp"

#include <Base/Fonts.hpp>
#include <Base/Profiling.hpp>
#include <Base/VectorMath.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

RadialMenu asdf;
int goldValue = 100;
Profiler::TimePoint mouseDown;

GameScreen::GameScreen() : Kunlaboro::Component("Game.GameScreen"),
	mMouseDown(false), mMenu(false), mSelectedTower(0), mTarget(nullptr)
{
}
void GameScreen::addedToEntity()
{
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
				auto resp = sendGlobalQuestion("Level.Valid", mTarget->mapPixelToCoords({ ev.mouseButton.x, ev.mouseButton.y }, mCamera));
				if (resp.handled && resp.payload.get<bool>())
				{
					mMenu = true;

					asdf.clearEntries();

					asdf.addEntry("Begin Wave", "Resources/Start.png");

					if (goldValue >= 25)
						asdf.addEntry("Build Tower (25G)", "Resources/Plus.png");

					asdf.setPosition({ float(ev.mouseButton.x), float(ev.mouseButton.y) });
					asdf.open();
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

							asdf.clearEntries();
							asdf.addEntry("Upgrade", "Resources/Plus.png");
							asdf.addEntry("Sell", "Resources/Plus.png");
							asdf.addEntry("Do something else", "Resources/Plus.png");

							asdf.setPosition({ float(ev.mouseButton.x), float(ev.mouseButton.y) });
							asdf.open();

							break;
						}
					}
				}
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
	asdf.update(dt);
	if (asdf.isClosed() && mMenu)
	{
		mMenu = false;

		if (mSelectedTower != 0)
		{
			if (asdf.getSelection() == "Sell")
			{
				sendMessageToEntity(mSelectedTower, "Sold");
				getEntitySystem()->destroyEntity(mSelectedTower);
			}

			mSelectedTower = 0;
		}
		else
		{
			if (asdf.getSelection() == "Build Tower (25G)")
			{
				goldValue -= 25;
				auto eid = getEntitySystem()->createEntity("Game.Tower");

				auto resp = sendGlobalQuestion("Level.CoordsToHex", mTarget->mapPixelToCoords(sf::Vector2i(asdf.getPosition()), mCamera));
				resp = sendGlobalQuestion("Level.HexToCoords", resp.payload.get<sf::Vector2i>());
				sendMessageToEntity(eid, "SetPosition", resp.payload.get<sf::Vector2f>());

				sendMessageToEntity(eid, "Define", &Towers::BasicTower::Instance);
			}
			else if (asdf.getSelection() == "Begin Wave")
			{
				sendMessage("NextWave");
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

	sf::Text gold("Gold: ", sf::getDefaultFont());
	gold.setPosition(10, target.getView().getSize().y - gold.getLocalBounds().height * 2);

	target.draw(gold);

	gold.move(gold.getLocalBounds().width, 0);
	gold.setString(std::to_string(goldValue));
	gold.setColor({ 255, 255, 0 });

	target.draw(gold);
}