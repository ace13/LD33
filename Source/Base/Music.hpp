#pragma once

#include "Tweening.hpp"

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <unordered_map>

class Engine;

class MusicManager
{
public:
	typedef unsigned int TrackID;

	class Track
	{
	public:
		Track(sf::SoundBuffer&& sound);
		Track(Track&& t);
		~Track();

		float getVolume() const;
		void setVolume(float volume, float duration = 0.f);
		float getPitch() const;
		void setPitch(float pitch);

	private:
		Track(const Track&) = delete;
		Track& operator=(const Track&) = delete;

		void update(float dt);

		Easer mVolumeTween;
		sf::Sound mTrack;
		sf::SoundBuffer mBuf;

		friend class MusicManager;
	};

	~MusicManager();

	void addTrack(TrackID id, sf::SoundBuffer&& sound);
	Track& operator[](TrackID);

	float getGlobalVolume() const;
	void setGlobalVolume(float);
	float getGlobalPitch() const;
	void setGlobalPitch(float);

private:
	MusicManager();
	MusicManager(MusicManager&&);

	MusicManager(const MusicManager&) = delete;
	MusicManager& operator=(const MusicManager&) = delete;

	void update(float dt);

	std::unordered_map<TrackID, Track> mTracks;

	friend class Engine;
};
