#pragma once

#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/View.hpp>

namespace sf { class RenderTarget; class Event; }

class GameScreen : public Kunlaboro::Component
{
public:
	GameScreen();

	void addedToEntity();

private:
	void event(sf::Event& ev);
	void update(float dt);
	void draw(sf::RenderTarget&);

	sf::Vector2i mMousePos;
	bool mMouseDown;
	float mCurZoom;

	sf::RenderTarget* mTarget;
	sf::View mCamera;
};