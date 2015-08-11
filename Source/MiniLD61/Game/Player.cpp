#include "Player.hpp"
#include "Components.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

Player::Player() : Kunlaboro::Component("Game.Player"),
	mFirst(true), mTime(0), mTargetX(0), mAnger(0), mPhysical(nullptr)
{
	mPlayerTex.loadFromFile("Resources/Player.png");
	mPlayerTex.setSmooth(true);
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
		auto ev = msg.payload.get<sf::Event*>();
		event(*ev);
	});
	requestMessage("LD33.Update", [this](const Kunlaboro::Message& msg){
		float dt = msg.payload.get<float>();
		update(dt);
	});
}

void Player::event(sf::Event& ev)
{
	if (ev.type == sf::Event::KeyPressed)
	{
		switch (ev.key.code)
		{
		case sf::Keyboard::A:
		case sf::Keyboard::Left:
			mTargetX = -950; break;
		case sf::Keyboard::D:
		case sf::Keyboard::Right:
			mTargetX = 950; break;

		case sf::Keyboard::Num1:
			mAnger = 0; break;
		case sf::Keyboard::Num2:
			mAnger = 1; break;
		case sf::Keyboard::Num3:
			mAnger = 2; break;

		default:
			break;
		}
	}
	else if (ev.type == sf::Event::KeyReleased)
	{
		switch (ev.key.code)
		{
		case sf::Keyboard::A:
		case sf::Keyboard::Left:
		case sf::Keyboard::D:
		case sf::Keyboard::Right:
			mTargetX = 0; break;

		default:
			break;
		}
	}

}

void Player::update(float dt)
{
	const sf::Vector2f CAMERA_OFFSET{ 0, 950 };

	float curX = mPhysical->getVelocity().x;
	mPhysical->setVelocity({ curX + (mTargetX - curX) * dt * 2, 0 });
	auto pos = mPhysical->getPosition();
	if (pos.x < mCameraSize.x / -8.f)
		pos.x = mCameraSize.x / -8.f;
	else if (pos.x > mCameraSize.x / 8.f)
		pos.x = mCameraSize.x / 8.f;

	mCameraPos += (pos - CAMERA_OFFSET - mCameraPos) * (dt * 2.5f);
	mTime += dt;
}

void Player::draw(sf::RenderTarget& target)
{
	auto view = target.getView();
	if (mFirst)
	{
		mFirst = false;
		mCameraPos = view.getCenter();
		mCameraSize = view.getSize();
	}

	view.setCenter(mCameraPos);
	target.setView(view);

	sf::Sprite player(mPlayerTex);
    player.setOrigin((sf::Vector2f)mPlayerTex.getSize() / 2.f);
    player.setPosition(mPhysical->getPosition());

    float xSpeed = mPhysical->getVelocity().x;
    player.setRotation(xSpeed / 200);

	sf::RectangleShape segment({ 25, 5 });
	segment.setOrigin({ 12.5f, 2.5f });
	segment.setRotation(player.getRotation());
	segment.setFillColor(sf::Color::Black);

	float rads = player.getRotation() * M_PI/180.f;
	sf::Vector2f xDir{
		std::cos(rads), std::sin(rads)
	}, yDir{
		std::cos(rads + float(M_PI_2)), std::sin(rads + float(M_PI_2))
	};

	segment.setPosition(mPhysical->getPosition() + 90.f * yDir);

    for (int i = 0; i < 25; ++i)
    {
    	if (i >= 22)
    		segment.scale(1.75f, 1);
    	if (i == 24)
    		segment.scale(0.4f, 1);

		target.draw(segment);
		segment.move(xDir * std::sin(i / (3.f + mAnger) + mTime * (2.5f + mAnger)*-1) + yDir * 5.f);

		segment.scale(0.93f, 1);
    }

    target.draw(player);
}

void Player::drawUI(sf::RenderTarget& target)
{

}
