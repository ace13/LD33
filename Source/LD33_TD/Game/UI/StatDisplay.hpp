#pragma once

#include <Base/Tweening.hpp>
#include <Kunlaboro/Component.hpp>

namespace sf { class RenderTarget; }

class StatDisplay : public Kunlaboro::Component
{
public:
	StatDisplay();

	void addedToEntity();

	void update(float dt);
	void draw(sf::RenderTarget&);

private:
	Easer mSizeEaser;
};
