#include "GameScreen.hpp"
#include "UI/TowerRadial.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

RadialMenu asdf;

GameScreen::GameScreen() : Kunlaboro::Component("Game.GameScreen"),
	mMouseDown(false), mMenu(false), mTarget(nullptr)
{
	asdf.addEntry("Build Tower", "Resources/Plus.png");
	asdf.addEntry("Build Tower2", "Resources/Plus.png");
	asdf.addEntry("Build Tower3", "Resources/Plus.png");
	asdf.addEntry("Build Tower4", "Resources/Plus.png");
	asdf.addEntry("Build Tower5", "Resources/Plus.png");
	asdf.addEntry("Build Tower6", "Resources/Plus.png");
	asdf.addEntry("Build Tower7", "Resources/Plus.png");
}
void GameScreen::addedToEntity()
{
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
		if (ev.mouseButton.button == sf::Mouse::Middle && asdf.isClosed())
		{
			mMouseDown = true;
			mMousePos = sf::Mouse::getPosition(*(sf::RenderWindow*)mTarget);
		}
	}
	else if (ev.type == sf::Event::MouseButtonReleased)
	{
		if (ev.mouseButton.button == sf::Mouse::Middle)
			mMouseDown = false;
		else if (ev.mouseButton.button == sf::Mouse::Left && asdf.isClosed())
		{
			mMenu = true;
			
			asdf.setPosition({ float(ev.mouseButton.x), float(ev.mouseButton.y) });
			asdf.open();
		}
		else if (ev.mouseButton.button == sf::Mouse::XButton1)
		{
			getEntitySystem()->createEntity("Game.Enemy");

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

		if (asdf.getSelection() == "Build Tower")
		{
			auto eid = getEntitySystem()->createEntity("Game.Tower");

			auto resp = sendGlobalQuestion("Level.CoordsToHex", mTarget->mapPixelToCoords(sf::Vector2i(asdf.getPosition()), mCamera));
			resp = sendGlobalQuestion("Level.HexToCoords", resp.payload.get<sf::Vector2i>());
			sendMessageToEntity(eid, "SetPosition", resp.payload.get<sf::Vector2f>());
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

		mCamera = target.getView();
		mCamera.zoom(0.25);
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
}