#include "MenuPane.hpp"

#include <Base/Fonts.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

const uint8_t SHADED_ALPHA = 24;
const float SHADED_OFFSET  = -32;
const uint8_t OPENED_ALPHA = 255;
const float OPENED_OFFSET  = 64;
const uint8_t CLOSED_ALPHA = 0;
const float CLOSED_OFFSET  = 256;

MenuPane::MenuPane() :
	mOffset(Tween::BackOut), mAlpha(Tween::LinearIn), mState(Closed), mLastJoy(0), mSelection(-1), mEntryCount(-1)
{
}

MenuPane::~MenuPane()
{
}

void MenuPane::draw(sf::RenderTarget& target)
{
	sf::Text title(getName(), sf::getDefaultFont(), 30U);

	title.setOrigin(0, 15);
	title.setPosition(getOffset(), 64);
	title.setColor(sf::Color(255,255,255,getAlpha()));

	target.draw(title);
}
void MenuPane::handleEvent(sf::Event& ev)
{
	if (mEntryCount <= 0 || (mState > MenuPane::Open && mState != MenuPane::Unshading))
		return;

	if (ev.type == sf::Event::KeyPressed)
	{
		if (ev.key.code == sf::Keyboard::Up || ev.key.code == sf::Keyboard::W)
		{
			if (--mSelection < 0)
				mSelection = mEntryCount - 1;
		}
		else if (ev.key.code == sf::Keyboard::Down || ev.key.code == sf::Keyboard::S)
		{
			if (++mSelection >= mEntryCount)
				mSelection = 0;
		}
	}
	else if (ev.type == sf::Event::JoystickMoved)
	{
		if (ev.joystickMove.axis == sf::Joystick::Axis::Y || ev.joystickMove.axis == sf::Joystick::Axis::PovY)
		{
			float len = ev.joystickMove.position;

			if (len < -50 && mLastJoy > -25)
			{
				if (--mSelection < 0)
					mSelection = mEntryCount - 1;
				mLastJoy = len;
			}
			else if (len > 50 && mLastJoy < 25)
			{
				if (++mSelection >= mEntryCount)
					mSelection = 0;
				mLastJoy = len;
			}
			else if (len > -25 || len < 25)
				mLastJoy = len;
		}
	}
}
void MenuPane::update(float dt)
{
	mOffset.update(dt);
	mAlpha.update(dt);

	if (mOffset.atEnd())
	{
		if (mState == Shading)
			mState = Shaded;
		else if (mState == Unshading || mState == Opening)
			mState = Open;
		else if (mState == Closing)
			mState = Closed;
	}
}

void MenuPane::setState(State s)
{
	const float ALPHA_DELAY = 0.5;
	const float OFFSET_DELAY = 0.75;

	mState = s;

	switch(mState)
	{
	case Shading:
		mOffset.setFunc(Tween::CubicOut);

		mAlpha.start(ALPHA_DELAY, OPENED_ALPHA, SHADED_ALPHA);
		mOffset.start(OFFSET_DELAY, OPENED_OFFSET, SHADED_OFFSET);
		break;

	case Shaded:
		mAlpha.start(0, SHADED_ALPHA, SHADED_ALPHA);
		mOffset.start(0, SHADED_OFFSET, SHADED_OFFSET);
		break;

	case Unshading:
		mOffset.setFunc(Tween::QuadraticIn);

		mAlpha.start(ALPHA_DELAY, SHADED_ALPHA, OPENED_ALPHA);
		mOffset.start(OFFSET_DELAY, SHADED_OFFSET, OPENED_OFFSET);
		break;

	case Closing:
		mOffset.setFunc(Tween::QuadraticIn);

		mAlpha.start(ALPHA_DELAY, OPENED_ALPHA, CLOSED_ALPHA);
		mOffset.start(OFFSET_DELAY, OPENED_OFFSET, CLOSED_OFFSET);
		break;

	case Closed:
		mAlpha.start(0, CLOSED_ALPHA, CLOSED_ALPHA);
		mOffset.start(0, CLOSED_OFFSET, CLOSED_OFFSET);
		break;

	case Opening:
		mOffset.setFunc(Tween::BackOut);

		mAlpha.start(ALPHA_DELAY, CLOSED_ALPHA, OPENED_ALPHA);
		mOffset.start(OFFSET_DELAY, -100, OPENED_OFFSET);
		break;

	case Open:
		mAlpha.start(0, OPENED_ALPHA, OPENED_ALPHA);
		mOffset.start(0, OPENED_OFFSET, OPENED_OFFSET);
		break;
	}
}
MenuPane::State MenuPane::getState() const
{
	return mState;
}

float MenuPane::getOffset() const
{
	return *mOffset;
}
float MenuPane::getAlpha() const
{
	return *mAlpha;
}

char MenuPane::getSelection() const
{
	return mSelection;
}
void MenuPane::setSelection(char entry)
{
	mSelection = entry;
}
void MenuPane::setEntryCount(char count)
{
	mEntryCount = count;
}
MenuState& MenuPane::getMenuState() const
{
	return *mMenu;
}

