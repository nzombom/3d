#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "math.hpp"
#include "shader.hpp"
#include "mesh.hpp"

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

	Mesh mesh = Mesh({
		{ { -1, -1, 0 }, { 0, 0, 1 } },
		{ { 1, -1, 0 }, { 0, 0, 1 } },
		{ { 1, 1, 0 }, { 0, 0, 1 } },
		{ { -1, 1, 0 }, { 0, 0, 1 } },
	}, { 0, 1, 2, 0, 2, 3 });

	Shader shader =
		Shader("resources/shaders/v.glsl", "resources/shaders/f.glsl");
	shader.use();

	Camera cam = { { 0, 0, 3 }, { 1, { 0, 0, 0 } },
			1, 8, M_PI / 2.0, 16.0 / 9.0 };

	while (!quit) {
		event();
		unsigned int t = SDL_GetTicks();

		float y = std::sin(t / 1000.0);
		float qr = std::sin(t / 2.0 / 1000.0);
		float qw = std::cos(t / 2.0 / 1000.0);
		shader.setVector("mTranslate", { 0, y, 0 });
		shader.setQuat("mRotate", { qw, { 0, qr, 0 } });
		shader.setVector("mScale", { 1, 1, 1 });
		shader.applyCamera(cam);

		glClearColor(0x0.2p0, 0x0.2p0, 0x0.2p0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		mesh.draw();

		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}
