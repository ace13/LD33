#pragma once

class TowerDefinition /// \TODO Name?
{
public:

	virtual float getFireRate() const = 0;
	
private:

};

namespace Towers
{
	class BasicTower : public TowerDefinition
	{
	public:
		float getFireRate() const { return 1; }

	private:
	};
}