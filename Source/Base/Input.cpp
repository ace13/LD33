#include "Input.hpp"

#include <cassert>

InputManager::InputManager()
{
}
InputManager::InputManager(InputManager&& rhs) :
	mBinds(std::move(rhs.mBinds))
{

}

void InputManager::setBind(unsigned int id, Bind&& bind)
{
	mBinds[id] = { std::move(bind) };
}

void InputManager::clearBind(unsigned int id)
{
	mBinds.erase(id);
}

void InputManager::linkBinds(unsigned int id, unsigned int lower, unsigned int upper)
{
	assert(id != lower && id != upper && "ID already in use");

	assert(mBinds.count(lower) != 0 && "No lower value");
	assert(mBinds.count(upper) != 0 && "No upper value");

	auto& lowerBind = mBinds[lower];
	auto& upperBind = mBinds[upper];
	auto& linkBind = mBinds[id];

	lowerBind.linkBind = &linkBind;
	linkBind.lowerBind = &lowerBind;
	upperBind.linkBind = &linkBind;
	linkBind.upperBind = &upperBind;
}

float InputManager::getValue(unsigned int bind) const
{
	///\TODO: if (notInFocus) return 0;
	return mBinds.at(bind).getValue();
}

void InputManager::handleEvent(sf::Event& ev)
{
	if (!(ev.type == sf::Event::KeyPressed || ev.type == sf::Event::KeyReleased ||
	    ev.type == sf::Event::JoystickButtonPressed || ev.type == sf::Event::JoystickButtonReleased ||
	    ev.type == sf::Event::JoystickMoved))
		return;

	for (auto& it : mBinds)
	{
		auto& b = it.second;

		switch (ev.type)
		{
		case sf::Event::KeyPressed:
		case sf::Event::KeyReleased:
			if (b.IBind.Type == Bind::KeyboardKey && b.IBind.Key.Code == ev.key.code)
			{
				float value = float(ev.type == sf::Event::KeyPressed);
				b.Value = value;
			} break;

		case sf::Event::JoystickButtonPressed:
		case sf::Event::JoystickButtonReleased:
			if (b.IBind.Type == Bind::JoystickButton &&
			    ev.joystickButton.joystickId == b.IBind.Button.Joystick)
			{
				float value = float(ev.type == sf::Event::JoystickButtonPressed);
				b.Value = value;
			} break;

		case sf::Event::JoystickMoved:
			if (b.IBind.Type == Bind::JoystickAxis &&
			    ev.joystickMove.joystickId == b.IBind.Axis.Joystick &&
			    ev.joystickMove.axis == b.IBind.Axis.Axis)
			{
				float pos = ev.joystickMove.position / 100.f;

				if (pos >= 0 && !b.IBind.Axis.Negative)
					b.Value = pos;
				else if (pos <= 0 && b.IBind.Axis.Negative)
					b.Value = -pos;
			} break;

		default: break;
		}
	}
}

InputManager::InternalBind::InternalBind(Bind&& bind) :
	IBind(bind), Value(0)
{
}
float InputManager::InternalBind::getValue() const
{
	if (lowerBind && upperBind)
		return upperBind->Value - lowerBind->Value;

	return Value;
}


