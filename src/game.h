#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <glad/glad.h>
#include <SDL.h>
#undef main
#include <SDL_opengl.h>
#include <map>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

#include "imgui_stuff.h"

class Game {
public:
	Game(uint16_t width, uint16_t height);
	~Game();
	void ProcessInput();
	void Draw() const;
	void Run();

private:
	Camera* m_Camera;
	float m_TimeOfDay;
	SDL_Window* m_SDLWindow;
	std::map<int, bool> m_Keyboard;
	bool m_FirstMouse = true;
	bool m_MouseEnabled = true;
	float m_DeltaTime;

};
#endif