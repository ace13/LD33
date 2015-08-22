#pragma once

#include <Kunlaboro/Component.hpp>
#include "Towers/Basic.hpp"

namespace Game { struct Physical; }
namespace sf { class RenderTarget; }

class Tower : public Kunlaboro::Component
{
public:
	Tower();

	void addedToEntity();

	void tick(float dt);
	void draw(sf::RenderTarget&);

protected:
	void levelUp(float cost = -1);

private:
	TowerDefinition* mDefinition;
	Game::Physical* mPhysical;

	float mXP;
	unsigned int mLevel;
};
