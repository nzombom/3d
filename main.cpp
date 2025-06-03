#include <iostream>
#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "math.hpp"

bool quit = false;

void event() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) quit = true;
	}
}

int main() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_Window * window = SDL_CreateWindow("3D", 960, 540, SDL_WINDOW_OPENGL);
	SDL_GL_CreateContext(window);
	glewInit();
	glViewport(0, 0, 960, 540);

	while (!quit) {
		event();

		glClearColor(0x0.2p0, 0x0.2p0, 0x0.2p0, 1);
		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}
