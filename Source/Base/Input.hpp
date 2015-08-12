#pragma once

#include <SFML/Window/Event.hpp>

#include <unordered_map>

class InputManager
{
public:
	~InputManager() = default;

	struct Bind
	{
		enum {
			KeyboardKey,
			JoystickButton,
			JoystickAxis
		} Type;

		union
		{
			struct {
				sf::Keyboard::Key Key;
			} KeyboardKey;
			struct {
				unsigned int Joystick;
				unsigned int Button;
			} JoystickButton;
			struct {
				unsigned int Joystick;
				sf::Joystick::Axis Axis;
				bool Negative;
			} JoystickAxis;
		};
	};

	void setBind(unsigned int id, Bind&& bind);
	void clearBind(unsigned int id);

	void linkBinds(unsigned int id, unsigned int lower, unsigned int upper);


private:
	struct InternalBind
	{
		Bind Bind;
		float Data;

		float getValue() const;

		InternalBind* linkBind,
			*lowerBind, *upperBind;
	};

	InputManager();
	InputManager(InputManager&&);
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

	void handleEvent(sf::Event&);

	std::unordered_map<unsigned int, InternalBind> mBinds;
};

