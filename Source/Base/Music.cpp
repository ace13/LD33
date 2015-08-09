#include "Music.hpp"


MusicManager::Track::Track(sf::Sound&& sound) :
	mVolumeTween(), mTrack(sound)
{
	mTrack.setLoop(true);
	mTrack.stop();
	mVolumeTween.start(0, 0, 0);
}
MusicManager::Track::~Track()
{

}

float MusicManager::Track::getVolume() const
{
	return mTrack.getVolume();
}
void MusicManager::Track::setVolume(float volume, float duration = 0.f)
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
MusicManager::~MusicManager()
{
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
