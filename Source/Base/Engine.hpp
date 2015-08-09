#pragma once

class Engine
{
public:
	Engine();
	Engine(Engine&&);
	~Engine();

	void run();

private:
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;


};
