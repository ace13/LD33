#pragma once

#include "Components.hpp"

#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf { class Event; class RenderTarget; }

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
	float mTime, mTailTime, mTargetX, mExcitement;
	sf::Texture mPlayerTex, mPlayerWingTex;
	sf::Vector2f mCameraPos, mCameraSize;
	Physical* mPhysical;

};
