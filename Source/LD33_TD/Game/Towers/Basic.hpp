#pragma once

class TowerDefinition /// \TODO Name?
{
public:

	virtual int getPrice() const = 0;
	virtual float getFireRate() const = 0;
	
private:

};

namespace Towers
{
	class BasicTower : public TowerDefinition
	{
	public:
		int getPrice() const { return 25; }
		float getFireRate() const { return 1; }

		static const BasicTower Instance;

	private:
	};
}