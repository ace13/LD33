#pragma once

#include "Path.hpp"
#include <Kunlaboro/Component.hpp>

namespace Game { struct Physical; }
namespace sf { class RenderTarget; }

class Enemy : public Kunlaboro::Component
{
public:
	Enemy();

	void addedToEntity();

	void tick(float dt);
	void draw(sf::RenderTarget&);

	void damage(float dmg);

private:
	float mStrength, mHealth;
	Game::Physical* mPhysical;
	Path* mPath;
	Path::iterator mPathIter;
};