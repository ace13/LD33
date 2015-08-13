#include "Player.hpp"
#include "Components.hpp"
#include "../Binds.hpp"

#include <Base/Engine.hpp>
#include <Base/Input.hpp>
#include <Base/Particles.hpp>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

Player::Player() : Kunlaboro::Component("Game.Player"),
	mFirst(true), mTime(0), mTargetX(0), mExcitement(0), mPhysical(nullptr)
{
	mPlayerWingTex.loadFromFile("Resources/PlayerAnim.png");
	mPlayerTex.loadFromFile("Resources/Player.png");
	mPlayerTex.setSmooth(true);
}

void Player::addedToEntity()
{
	requireComponent("Game.Physical", [this](const Kunlaboro::Message& msg){
		auto comp = dynamic_cast<Physical*>(msg.sender);

		// We want the player to look good,
		// so let's run his physics updates every frame.
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
	changeRequestPriority("LD33.Draw", 1);
	requestMessage("LD33.DrawUI", [this](const Kunlaboro::Message& msg){
		auto& target = *msg.payload.get<sf::RenderTarget*>();
		drawUI(target);
	});
	requestMessage("LD33.Update", [this](const Kunlaboro::Message& msg){
		float dt = msg.payload.get<float>();
		update(dt);
	});
}

void Player::update(float dt)
{
	mTargetX = Engine::get<InputManager>().getValue(Bind_Horizontal) * 950;
	const sf::Vector2f CAMERA_OFFSET{ 0, 950 };

	float curX = mPhysical->getVelocity().x;

	auto pos = mPhysical->getPosition();
	if (pos.x < mCameraSize.x / -16.f)
	{
		if (pos.x < mCameraSize.x / -2.f)
		{
			mTargetX = 500;
			mPhysical->setPosition({ mCameraSize.x / -2.f, mPhysical->getPosition().y });
		}
		pos.x = mCameraSize.x / -16.f;
	}
	else if (pos.x > mCameraSize.x / 16.f)
	{
		if (pos.x > mCameraSize.x / 2.f)
		{
			mTargetX = -500;
			mPhysical->setPosition({ mCameraSize.x / 2.f, mPhysical->getPosition().y });
		}
		pos.x = mCameraSize.x / 16.f;
	}

	mPhysical->setVelocity({ curX + (mTargetX - curX) * dt * 2, 0 });
	mCameraPos += (pos - CAMERA_OFFSET - mCameraPos) * (dt * 2.5f);
	mTime += dt;

	float targetExcitmenet = 1;

	if (Engine::get<InputManager>().getValue(Bind_Fire) > 0.5f)
	{
		targetExcitmenet = 3;
		auto p = Particles::CloudPuff;

		float rads = mPhysical->getVelocity().x * float(M_PI) / 18000.f;
		sf::Vector2f yDir{
			std::cos(rads + float(M_PI_2)), std::sin(rads + float(M_PI_2))
		};

		//\TODO: Fix the random in this

 		p.Layer = ParticleManager::Particle::Default;
		p.Duration = 0.5f + float(rand()%50 - 25) / 100.f;
		p.StartScale = 0.05f;
		p.EndScale = 0.25f;
		p.StartColor = { 255, 200, 25 };
		p.EndColor = { 120, 5, 0 };
		p.Velocity = mPhysical->getVelocity() + sf::Vector2f{ 100 - float(rand()%200), -1000 };
		p.Gravity = { float(rand() % 100) - 50, 1000 + float(rand() % 1000) };

		Engine::get<ParticleManager>().addParticle(p, mPhysical->getPosition() + (-115.f * yDir));
	}

	mExcitement += (targetExcitmenet - mExcitement) * dt;
	mTailTime += dt * mExcitement * 2;
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
    player.setOrigin({ 125.f, 125.f });
    player.setPosition(mPhysical->getPosition());

    float xSpeed = mPhysical->getVelocity().x;
    player.setRotation(xSpeed / 100);
    target.draw(player);

	int tex = int(mTime * 2.f) % 4;
	player.setTexture(mPlayerWingTex);
	player.setTextureRect({ tex % 2 * 250, tex / 2 * 250, 250, 250 });
	target.draw(player);

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
		segment.move(
		    mPhysical->getVelocity() / -500.f +
		    xDir * std::sin(i / (3.f + mExcitement) -
		    mTailTime) +
		    yDir * 5.f
		);

		segment.scale(0.93f, 1);
    }

}

void Player::drawUI(sf::RenderTarget& target)
{

}
