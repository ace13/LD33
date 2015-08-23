#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>

namespace Game { struct Physical; }
namespace sf { class RenderTarget; }

class TowerDefinition /// \TODO Name?
{
public:

	virtual std::string getName() const = 0;

	virtual int getPrice() const = 0;
	virtual float getDamage(unsigned int level) const = 0;
	virtual float getFireRate(unsigned int level) const = 0;

	virtual void fire(const Game::Physical& source, Game::Physical& target) const = 0;
	virtual void draw(const Game::Physical& position, const Game::Physical* target, sf::RenderTarget& rt) const = 0;
	
private:

};

namespace Towers
{
	class Basic : public TowerDefinition
	{
	public:
		std::string getName() const { return "Water Pistol"; }

		int getPrice() const { return 25; }
		float getDamage(unsigned int level) const { return 0.5 + (level/10.f); }
		float getFireRate(unsigned int) const { return 0.1f; }

		void fire(const Game::Physical& source, Game::Physical& target) const;
		void draw(const Game::Physical& position, const Game::Physical* target, sf::RenderTarget& rt) const;

		static const Basic Instance;

	private:
	};

	class Firehose : public TowerDefinition
	{
	public:
		std::string getName() const { return "Firehose"; }

		int getPrice() const { return 75; }
		float getDamage(unsigned int level) const { return (level / 20.f); }
		float getFireRate(unsigned int) const { return 0.001f; }

		void fire(const Game::Physical& source, Game::Physical& target) const;
		void draw(const Game::Physical& position, const Game::Physical* target, sf::RenderTarget& rt) const;

		static const Firehose Instance;

	private:
	};
}