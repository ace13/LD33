#include "GameState.hpp"

#include <Base/Engine.hpp>
#include <Base/Particles.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <random>

GameState::GameState() : Kunlaboro::Component("States.Game"),
	mCloudTimer(0)
{
}

void GameState::addedToEntity()
{
	getEntitySystem()->createEntity("Game.Player");

	requestMessage("LD33.Event", [this](const Kunlaboro::Message& msg){
		event(*msg.payload.get<sf::Event*>());
	});
	requestMessage("LD33.Tick", [this](const Kunlaboro::Message& msg){
		tick(msg.payload.get<float>());
	});
	requestMessage("LD33.Draw", [this](const Kunlaboro::Message& msg){
		draw(*msg.payload.get<sf::RenderTarget*>());
	});
	changeRequestPriority("LD33.Draw", -129);
}

void GameState::event(sf::Event& ev)
{
	if (ev.type == sf::Event::KeyReleased && ev.key.code == sf::Keyboard::Escape)
		Engine::get<Engine>().close();
}

void GameState::tick(float dt)
{
	mCloudTimer -= dt;
	if (mCloudTimer <= 0)
	{
		const float CLOUD_RATE = 1.f;
		const float CLOUD_RANDOM = 0.9f;

		std::random_device dev;
		std::uniform_real_distribution<float> time(-CLOUD_RANDOM, CLOUD_RANDOM);

		addCloud();
		mCloudTimer = CLOUD_RATE + time(dev);
	}
}

void GameState::draw(sf::RenderTarget& target)
{
	target.clear({ 22, 67, 126 });
}

void GameState::addCloud()
{
	auto& particles = Engine::get<ParticleManager>();
	std::random_device rd;

	std::uniform_real_distribution<float> dist(-1, 1);
    std::uniform_int_distribution<int> type(0, 3);
    std::uniform_int_distribution<int> puffs(9, 34);
    std::uniform_int_distribution<int> color(152, 222);

    float baseXCoord = dist(rd) * 2000.f;

    int j = puffs(rd);
    for (int i = 0; i < j; ++i)
    {
        auto p = Particles::CloudPuff;
<<<<<<< Updated upstream
        p.Angle = dist(rd) * M_PI;
=======
        p.Angle = dist(rd) * float(M_PI);
>>>>>>> Stashed changes
        p.Rotation = dist(rd) * 0.05f;
        p.StartScale = 1.75f + dist(rd) * 0.75f;
        p.EndScale = 1.75f + dist(rd) * 0.75f;

        int t = type(rd);
<<<<<<< Updated upstream
        p.Rect.top = 100 + (t / 2) * 200;
        p.Rect.left = 100 + (t % 2) * 200;
=======
        p.Rect.top = float(100 + int(t / 2) * 200);
        p.Rect.left = float(100 + (t % 2) * 200);
>>>>>>> Stashed changes

        uint8_t col = color(rd);
        p.StartColor = sf::Color(col,col,col,79);
        col = color(rd);
        p.EndColor = sf::Color(col,col,col,79);

    	particles.addParticle(std::move(p), sf::Vector2f(baseXCoord, -2600) + sf::Vector2f(dist(rd) * 390, dist(rd) * 180));
    }
}

