#include "Wave.hpp"

const WaveManager::Wave WaveManager::Wave::None{
	0, 0, 0, "No Wave"
};

WaveManager::WaveManager() : Kunlaboro::Component("Game.WaveManager"),
	mAccum(0), mSpawned(0)
{
	mNextWave = mCurWave = mWaves.end();
}

void WaveManager::addedToEntity()
{
	requestMessage("LD33.Tick", [this](const Kunlaboro::Message& msg) { tick(msg.payload.get<float>()); });
	requestMessage("Game.CurrentWave", [this](Kunlaboro::Message& msg) {
		msg.handle(*mCurWave);
	});
	requestMessage("NextWave", [this](const Kunlaboro::Message& msg) {
		nextWave();
	}, true);
}

void WaveManager::addWave(const Wave& wave)
{
	mWaves.push_back(wave);

	mNextWave = mWaves.begin();
	mCurWave = mWaves.end();
}
void WaveManager::nextWave()
{
	if (mCurWave != mWaves.end())
		return;

	if (mNextWave != mWaves.end())
	{
		mSpawned = 0;
		mCurWave = mNextWave++;
		mAccum = mCurWave->SpawnRate;
	}
}

const WaveManager::Wave& WaveManager::curWave() const
{
	if (mCurWave == mWaves.end())
		return Wave::None;

	return *mCurWave;
}
bool WaveManager::atEnd() const
{
	return mNextWave == mWaves.end();
}

void WaveManager::tick(float dt)
{
	if (mCurWave != mWaves.end())
	{
		mAccum += dt;
		if (mAccum >= mCurWave->SpawnRate)
		{
			auto eid = getEntitySystem()->createEntity("Game.Enemy");
			getEntitySystem()->sendLocalMessage(eid, "SetStrength", mCurWave->Strength);

			mAccum -= mCurWave->SpawnRate;
			++mSpawned;
		}

		if (mSpawned >= mCurWave->Number)
			mCurWave = mWaves.end();
	}
}