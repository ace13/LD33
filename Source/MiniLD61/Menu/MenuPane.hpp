#pragma once

#include <Base/Tweening.hpp>

#include <string>

namespace sf
{
<<<<<<< Updated upstream
	struct Event;
=======
	class Event;
>>>>>>> Stashed changes
	class RenderTarget;
}

class MenuState;
class MenuPane
{
public:
	enum State
	{
		Opening,   // Coming in from the left
		Open,      // Pane is open in the foreground
		Shading,   // Sliding off to the left
		Shaded,    // In the background on the left
		Unshading, // Sliding back into focus
		Closing,   // Disappearing off to the right
		Closed     // Gone.
	};

	MenuPane();
	virtual ~MenuPane();

	virtual void draw(sf::RenderTarget&);
	virtual void handleEvent(sf::Event&);
	virtual void update(float dt);

	virtual std::string getName() const = 0;

	void setState(State s);
	State getState() const;

protected:
	float getOffset() const;
	float getAlpha() const;

	char getSelection() const;
	void setSelection(char entry);
	void setEntryCount(char count);

	MenuState& getMenuState() const;

private:
	MenuState* mMenu;
	Easer mOffset, mAlpha;
	State mState;

	float mLastJoy;
	char mSelection, mEntryCount;

	friend class MenuState;
};

