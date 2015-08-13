#pragma once

namespace Kunlaboro { class EntitySystem; }
namespace sf { class RenderWindow; }

class Engine
{
public:
	static const float TICK_RATE;

	Engine();
	Engine(Engine&&);
	~Engine();

	void setSystem(Kunlaboro::EntitySystem& sys);
	void setWindow(sf::RenderWindow& window);
	void run();

	void close();

	template<typename T>
	static T& get();

private:
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	sf::RenderWindow* mWindow;
	Kunlaboro::EntitySystem* mSystem;
};
