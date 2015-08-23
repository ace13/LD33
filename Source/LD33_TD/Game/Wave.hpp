#pragma once

#include <Kunlaboro/Component.hpp>

#include <list>

class WaveManager : public Kunlaboro::Component
{
public:
	struct Wave
	{
		unsigned int Number;
		float Strength;
		float SpawnRate;
		std::string Name;

		static const Wave None;
		inline bool operator==(const Wave& o) { return Name == o.Name; }
	};

	WaveManager();
	
	void addedToEntity();

	void addWave(const Wave& wave);
	void nextWave();

	const Wave& getCurWave() const;
	const Wave& getNextWave() const;

	bool atEnd() const;

private:
	void tick(float dt);

	float mAccum;
	unsigned int mSpawned;

	std::list<Wave> mWaves;
	std::list<Wave>::iterator mCurWave, mNextWave;
};
