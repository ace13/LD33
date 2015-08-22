#include "Music.hpp"


MusicManager::Track::Track(sf::SoundBuffer&& sound) :
	mVolumeTween(), mBuf(std::move(sound))
{
	mTrack.setBuffer(mBuf);
	mTrack.setVolume(0);
	mTrack.setLoop(true);
	mTrack.stop();
	mVolumeTween.start(0, 0, 0);
}
MusicManager::Track::Track(Track&& t) :
	mVolumeTween(std::move(t.mVolumeTween)), mTrack(std::move(t.mTrack)), mBuf(std::move(t.mBuf))
{
	mTrack.setBuffer(mBuf);
}

MusicManager::Track::~Track()
{

}

float MusicManager::Track::getVolume() const
{
	return mTrack.getVolume();
}
void MusicManager::Track::setVolume(float volume, float duration)
{
	mVolumeTween.start(mTrack.getVolume(), volume, duration);
}
float MusicManager::Track::getPitch() const
{
	return mTrack.getPitch();
}
void MusicManager::Track::setPitch(float pitch)
{
	mTrack.setPitch(pitch);
}
void MusicManager::Track::update(float dt)
{
	mVolumeTween.update(dt);
	float volume = *mVolumeTween,
	      oldVolume = mTrack.getVolume();
	
	if (volume != oldVolume)
	{
		mTrack.setVolume(volume);

		if (volume > 0 && oldVolume == 0)
			mTrack.play();
		else if (volume == 0 && oldVolume > 0)
			mTrack.stop();
	}
}

MusicManager::MusicManager()
{

}
MusicManager::MusicManager(MusicManager&& other) :
	mTracks(std::move(other.mTracks))
{
}
MusicManager::~MusicManager()
{
}

void MusicManager::addTrack(TrackID id, sf::SoundBuffer&& sound)
{
	mTracks.emplace(id, Track(std::move(sound)));
}

MusicManager::Track& MusicManager::operator[](TrackID id)
{
	return mTracks.at(id);
}

float MusicManager::getGlobalVolume() const
{
	return 1;
}
void MusicManager::setGlobalVolume(float)
{

}
float MusicManager::getGlobalPitch() const
{
	return 1;
}
void MusicManager::setGlobalPitch(float)
{

}
void MusicManager::update(float dt)
{
	for (auto& track : mTracks)
		track.second.update(dt);
}
