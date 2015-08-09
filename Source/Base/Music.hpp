#pragma once

#include "Tweening.hpp"

#include <SFML/Audio/Sound.hpp>

#include <unordered_map>

class Engine;

class MusicManager
{
public:
	typedef unsigned int TrackID;

	class Track
	{
	public:
		~Track();

		float getVolume() const;
		void setVolume(float volume, float duration = 0.f);
		float getPitch() const;
		void setPitch(float pitch);

	private:
		Track(sf::Sound&& sound);

		Track(const Track&) = delete;
		Track& operator=(const Track&) = delete;

		void update(float dt);

		Easer mVolumeTween;
		sf::Sound mTrack;

		friend class MusicManager;
	};

	~MusicManager();

	Track& operator[](TrackID);

	float getGlobalVolume() const;
	void setGlobalVolume(float);
	float getGlobalPitch() const;
	void setGlobalPitch(float);

private:
	MusicManager();

	MusicManager(const MusicManager&) = delete;
	MusicManager& operator=(const MusicManager&) = delete;

	void update(float dt);

	std::unordered_map<TrackID, Track> mTracks;

	friend class Engine;
};
