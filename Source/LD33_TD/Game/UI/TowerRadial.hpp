#pragma once

#include <Base/Tweening.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>
#include <list>

namespace sf { class Event;  class RenderTarget; }

class RadialMenu
{
public:
	RadialMenu();

	void open();
	void close();

	void clearEntries();
	void addEntry(const std::string& name, const std::string& file);
	bool hasEntries() const;

	void setPosition(const sf::Vector2f&);
	const sf::Vector2f& getPosition() const;

	void event(sf::Event&);
	void update(float dt);
	void draw(sf::RenderTarget& target);

	bool isClosed() const;
	bool isOpened() const;
	std::string getSelection() const;

private:
	enum {
		State_Closed,
		State_Opening,
		State_Open,
		State_Closing
	} mState;

	Easer mSizeEaser, mRotationEaser;
	std::list<std::pair<std::string, sf::Texture>> mEntries;
	sf::Vector2f mPosition;
	std::string mSelected;

	sf::RenderTarget* mTarget;
	sf::View mView;
};