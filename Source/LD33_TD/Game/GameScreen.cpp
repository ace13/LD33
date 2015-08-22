#include "GameScreen.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

GameScreen::GameScreen() : Kunlaboro::Component("Game.GameScreen"),
	mMouseDown(false), mTarget(nullptr)
{

}
void GameScreen::addedToEntity()
{
	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg) { draw(*msg.payload.get<sf::RenderTarget*>()); });
	changeRequestPriority("LD33.Draw", -9001);
}
void GameScreen::event(sf::Event& ev)
{
	if (ev.type == sf::Event::MouseButtonPressed)
	{
		if (ev.mouseButton.button == sf::Mouse::Middle)
		{
			mMouseDown = true;
			mMousePos = sf::Mouse::getPosition(*(sf::RenderWindow*)mTarget);
		}
	}
	else if (ev.type == sf::Event::MouseButtonReleased)
	{
		if (ev.mouseButton.button == sf::Mouse::Middle)
			mMouseDown = false;
		else if (ev.mouseButton.button == sf::Mouse::Left)
		{
			auto eid = getEntitySystem()->createEntity("Game.Tower");

			auto resp = sendGlobalQuestion("Level.CoordsToHex", mTarget->mapPixelToCoords(sf::Mouse::getPosition(*(sf::RenderWindow*)mTarget), mCamera));
			resp = sendGlobalQuestion("Level.HexToCoords", resp.payload.get<sf::Vector2i>());
			sendMessageToEntity(eid, "SetPosition", resp.payload.get<sf::Vector2f>());
		}
		else if (ev.mouseButton.button == sf::Mouse::XButton1)
		{
			getEntitySystem()->createEntity("Game.Enemy");

		}
	}
	else if (ev.type == sf::Event::MouseWheelMoved)
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
}