#pragma once

class InputManager
{
public:
	~InputManager() = default;



private:
	InputManager();
	InputManager(InputManager&&);
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;



};

