#include "Components.hpp"

Physical::Physical() : Kunlaboro::Component("Game.Physical")
{

}

void Physical::addedToEntity()
{
	requestMessage("LD33.Tick", [this](const Kunlaboro::Message& msg){
	   tick(msg.payload.get<float>());
	});
}

void Physical::tick(float dt)
{
	mPosition += mVelocity * dt;
}

const sf::Vector2f& Physical::getPosition() const
{
	return mPosition;
}
const sf::Vector2f& Physical::getSize() const
{
	return mSize;
}
const sf::Vector2f& Physical::getVelocity() const
{
	return mVelocity;
}
void Physical::setPosition(const sf::Vector2f& pos)
{
	mPosition = pos;
}
void Physical::setSize(const sf::Vector2f& size)
{
	mSize = size;
	mRadiusSquared = (size.x*size.x)+(size.y*size.y);
}
void Physical::setVelocity(const sf::Vector2f& vel)
{
	mVelocity = vel;
}

