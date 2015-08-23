#include "TowerRadial.hpp"

#include <Base/Fonts.hpp>
#include <Base/VectorMath.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

RadialMenu::RadialMenu() :
	mState(State_Closed), mSizeEaser(Tween::BackOut), mRotationEaser(Tween::BounceOut)
{

}

void RadialMenu::open()
{
	mState = State_Opening;
	mSizeEaser.start(0.f, 1.f, 2.5f);
	mRotationEaser.start(0.f, 1.f, 1.5f);
}
void RadialMenu::close()
{
	mState = State_Closing;
	mSizeEaser.setFunc(Tween::QuinticIn);
	mSizeEaser.start(1.f, 0.f, 0.75f);
}

void RadialMenu::addEntry(const std::string& name, const std::string& file)
{
	sf::Texture tex;
	if (tex.loadFromFile(file))
		mEntries.emplace_back(name, std::move(tex));
}

void RadialMenu::event(sf::Event& ev)
{
	if (!mTarget || mState >= State_Closing)
		return;

	std::list<std::pair<std::string, sf::Vector2f>> entryPos;
	if (ev.type == sf::Event::MouseMoved || ev.type == sf::Event::MouseButtonReleased)
	{
		size_t i = 0, j = mEntries.size();
		for (auto& ent : mEntries)
		{
			float ang = -float(M_PI_2) + (float(M_PI * 2) / j) * i * *mRotationEaser;
			entryPos.emplace_back(ent.first, mPosition + sf::Vector2f(std::cos(ang) * 44.f, std::sin(ang) * 44.f) * *mSizeEaser);
			++i;
		}
	}

	if (ev.type == sf::Event::MouseMoved)
	{
		mSelected = "";
		for (auto& pos : entryPos)
		{
			auto mPos = mTarget->mapPixelToCoords({ ev.mouseMove.x, ev.mouseMove.y }, mView);

			if (VMath::Distance(mPos, pos.second) < 16 * 16)
			{
				mSelected = pos.first;
				return;
			}
		}
	}
	else if (ev.type == sf::Event::MouseButtonReleased && ev.mouseButton.button == sf::Mouse::Left)
	{
		close();
	}
}

void RadialMenu::update(float dt)
{
	mSizeEaser.update(dt);
	mRotationEaser.update(dt);

	if (mSizeEaser.atEnd())
	{
		switch (mState)
		{
		case RadialMenu::State_Opening:
			mState = State_Open;

			mSizeEaser.setFunc(Tween::QuinticIn);
			mSizeEaser.start(0.995f, 1.005f, 1.f);
			mSizeEaser.update(0.125f);
			break;

		case RadialMenu::State_Open:
			mSizeEaser.reset(true);
			break;

		case RadialMenu::State_Closing:
			mState = State_Closed;
			break;
		}
	}

}
void RadialMenu::draw(sf::RenderTarget& target)
{
	if (mState == State_Closed)
		return;

	mTarget = &target;
	mView = target.getView();

	sf::CircleShape radial;

	radial.setScale(*mSizeEaser, *mSizeEaser);

	radial.setPosition(mPosition);

	radial.setRadius(40);
	radial.setOrigin(40, 40);

	radial.setFillColor(sf::Color::Transparent);
	radial.setOutlineColor(sf::Color::Black);
	radial.setOutlineThickness(8);
	
	target.draw(radial);

	radial.setOutlineThickness(4);
	radial.setOutlineColor({ 128, 128, 128 });
	radial.setRadius(44);
	radial.setOrigin(44, 44);

	target.draw(radial);

	radial.setOutlineColor(sf::Color::Black);
	radial.setFillColor(sf::Color::White);
	radial.setRadius(16);
	radial.setOrigin(16, 16);

	sf::Sprite sprite;
	sprite.setScale(*mSizeEaser, *mSizeEaser);

	size_t i = 0, j = mEntries.size();
	for (auto& entry : mEntries)
	{
		float ang = -float(M_PI_2) + (float(M_PI * 2) / j) * i * *mRotationEaser;
		float scale = *mSizeEaser;

		if (entry.first == mSelected)
		{
			radial.setFillColor({ 255, 197, 197 });
			sprite.setScale(scale + 0.25f, scale + 0.25f);
			radial.setScale(scale + 0.25f, scale + 0.25f);
		}
		else
		{
			radial.setFillColor({ 255, 255, 255 });
			radial.setScale(scale, scale);
			sprite.setScale(scale, scale);
		}

		radial.setPosition(mPosition + sf::Vector2f(std::cos(ang) * 44.f, std::sin(ang) * 44.f) * *mSizeEaser);
		sprite.setPosition(radial.getPosition());

		target.draw(radial);

		sprite.setTexture(entry.second);
		sprite.setOrigin(sf::Vector2f(entry.second.getSize()) / 2.f);

		target.draw(sprite);

		if (entry.first == mSelected)
		{
			sf::Text popup(entry.first, sf::getDefaultFont());
			popup.setPosition(sprite.getPosition() - sf::Vector2f(0, 32));
			popup.setOrigin(0, popup.getLocalBounds().height);

			target.draw(popup);
		}

		++i;
	}
}

bool RadialMenu::isClosed() const
{
	return mState == State_Closed;
}
std::string RadialMenu::getSelection() const
{
	return mSelected;
}