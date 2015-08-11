#include "Player.hpp"
#include "Components.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

Player::Player() : Kunlaboro::Component("Game.Player"),
	mFirst(true), mPhysical(nullptr)
{
	mPlayerTex.loadFromFile("Resources/Player.png");
}

void Player::addedToEntity()
{
	requireComponent("Game.Physical", [this](const Kunlaboro::Message& msg){
		auto comp = dynamic_cast<Physical*>(msg.sender);

		// We want the player to look good,
		// so let's run his updates every frame.
		comp->unrequestMessage("LD33.Tick");
		comp->requestMessage("LD33.Update", [comp](const Kunlaboro::Message& msg){
			comp->tick(msg.payload.get<float>());
		});

		mPhysical = comp;
	});

	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg){
		auto& target = *msg.payload.get<sf::RenderTarget*>();
		draw(target);
	});
	requestMessage("LD33.DrawUI", [this](const Kunlaboro::Message& msg){
		auto& target = *msg.payload.get<sf::RenderTarget*>();
		drawUI(target);
	});
	requestMessage("LD33.Event", [this](const Kunlaboro::Message& msg){
		auto ev = msg.payload.get<sf::Event>();
		event(ev);
	});
	requestMessage("LD33.Update", [this](const Kunlaboro::Message& msg){
		float dt = msg.payload.get<float>();
		update(dt);
	});
}

void Player::event(sf::Event& ev)
{

}

void Player::update(float dt)
{
	const sf::Vector2f CAMERA_OFFSET{ 0, 1020 };
	mCameraPos += (mPhysical->getPosition() - CAMERA_OFFSET - mCameraPos) * (dt * 2.5f);
}

void Player::draw(sf::RenderTarget& target)
{
	auto view = target.getView();
	if (mFirst)
	{
		mFirst = false;
		mCameraPos = view.getCenter();
	}

	view.setCenter(mCameraPos);
	target.setView(view);

	sf::Sprite player(mPlayerTex);
    player.setOrigin((sf::Vector2f)mPlayerTex.getSize() / 2.f);
    player.setPosition(mPhysical->getPosition());

    target.draw(player);
}

void Player::drawUI(sf::RenderTarget& target)
{

}
