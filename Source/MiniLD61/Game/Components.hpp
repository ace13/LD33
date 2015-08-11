#pragma once

#include <Kunlaboro/Component.hpp>
#include <SFML/System/Vector2.hpp>


class Physical : public Kunlaboro::Component
{
public:
	Physical();
	~Physical() = default;

	void addedToEntity();

	void tick(float dt);

	const sf::Vector2f& getPosition() const;
	const sf::Vector2f& getSize() const;
	const sf::Vector2f& getVelocity() const;
	void setPosition(const sf::Vector2f& pos);
	void setSize(const sf::Vector2f& size);
	void setVelocity(const sf::Vector2f& vel);

private:
	sf::Vector2f mPosition, mSize, mVelocity;
	float mRadiusSquared;
};

