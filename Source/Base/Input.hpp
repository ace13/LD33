#pragma once

#include <SFML/Window/Event.hpp>

#include <unordered_map>

class Engine;

class InputManager
{
public:
	~InputManager() = default;

	struct Bind
	{
		struct KeyBind {
			sf::Keyboard::Key Code;
		};
		struct JoyButtonBind {
			unsigned int Joystick;
			unsigned int Button;
		};
		struct JoyAxisBind {
			unsigned int Joystick;
			sf::Joystick::Axis Axis;
			bool Negative;
		};


		enum {
			None,

			KeyboardKey,
			JoystickButton,
			JoystickAxis
		} Type;

		union
		{
			KeyBind Key;
			JoyButtonBind Button;
			JoyAxisBind Axis;
		};

		Bind() : Type(None) { }

		Bind(const KeyBind& bind) : Type(KeyboardKey), Key(bind) { }
		Bind(const JoyButtonBind& bind) : Type(JoystickButton), Button(bind) { }
		Bind(const JoyAxisBind& bind) : Type(JoystickAxis), Axis(bind) { }
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

	friend class Engine;
};

