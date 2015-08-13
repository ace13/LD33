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
			None,
			KeyboardKey,
			JoystickButton,
			JoystickAxis
		} Type;

		union
		{
			struct {
				sf::Keyboard::Key Code;
			} Key;
			struct {
				unsigned int Joystick;
				unsigned int Button;
			} Button;
			struct {
				unsigned int Joystick;
				sf::Joystick::Axis Axis;
				bool Negative;
			} Axis;
		};
	};

	void setBind(unsigned int id, Bind&& bind);
	void clearBind(unsigned int id);

	void linkBinds(unsigned int id, unsigned int lower, unsigned int upper);

	float getValue(unsigned int bind) const;

private:
	struct InternalBind
	{
		InternalBind() = default;
		InternalBind(Bind&&);

		InputManager::Bind IBind;
		float Value;

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

