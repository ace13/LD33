#pragma once

#include "Components.hpp"

#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf { struct Event; class RenderTarget; }

class Player : public Kunlaboro::Component
{
public:
	Player();
	~Player() = default;

	void addedToEntity();

	void event(sf::Event&);
	void update(float dt);
	void draw(sf::RenderTarget&);
	void drawUI(sf::RenderTarget&);

private:
	bool mFirst;
	float mTime, mTargetX, mAnger;
	sf::Texture mPlayerTex;
	sf::Vector2f mCameraPos, mCameraSize;
	Physical* mPhysical;

};
