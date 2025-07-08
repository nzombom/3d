#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "math.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "meshUtils.hpp"
#include "renderer.hpp"
#include "input.hpp"

bool quit = false;
std::vector<InputControl> controls = {
	{ INPUT_KEYBOARD, { .k = SDLK_W } },
	{ INPUT_KEYBOARD, { .k = SDLK_S } },
	{ INPUT_KEYBOARD, { .k = SDLK_A } },
	{ INPUT_KEYBOARD, { .k = SDLK_D } },
	{ INPUT_KEYBOARD, { .k = SDLK_Q } },
	{ INPUT_KEYBOARD, { .k = SDLK_E } },
};
InputState input = InputState(controls);

void event() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) switch (e.type) {
		case SDL_EVENT_QUIT:
			quit = true;
			break;
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
			input.update(e);
			break;
	}
}

int main() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_Window * window = SDL_CreateWindow("3D", 960, 540, SDL_WINDOW_OPENGL);
	SDL_GL_CreateContext(window);
	glewInit();
	glViewport(0, 0, 960, 540);

	Shader bufferShader("shaders/bufferv.glsl",
			"shaders/bufferf.glsl");
	Shader deferredScreenShader("shaders/deferredv.glsl",
			"shaders/deferredf.glsl");
	Shader lightShader("shaders/bufferv.glsl",
			"shaders/lightf.glsl");

	bufferShader.compile();
	deferredScreenShader.compile();
	lightShader.compile();

	Renderer r(bufferShader, deferredScreenShader);

	Mesh cube = generateCube();
	Mesh sphere = generateSphere(16);
	Mesh lightSphere = generateSphere(8);

	vector cPos = { 0, 0, 4 };

	unsigned int prev = 0;
	while (!quit) {
		event();
		unsigned int t = SDL_GetTicks();
		unsigned int dt = t - prev;
		prev = t;

		if (input.s(0)) cPos -= vector{ 0, 0, 0.1 };
		if (input.s(1)) cPos += vector{ 0, 0, 0.1 };
		if (input.s(2)) cPos -= vector{ 0.1, 0, 0 };
		if (input.s(3)) cPos += vector{ 0.1, 0, 0 };
		if (input.s(4)) cPos -= vector{ 0, 0.1, 0 };
		if (input.s(5)) cPos += vector{ 0, 0.1, 0 };
		Camera cam = { cPos, { 1, { 0, 0, 0 } },
			1, 64, M_PI / 2.0, 16.0 / 9.0 };

		Transform bgt = { { 0, 0, 0 }, IDR, IDS * 4 };
		float y = std::cos(t / 1000.0);
		Transform st = { { 0, y * 2, 0 }, IDR, { 1, 1, 1 }, };
		float x = std::sin(t / 1000.0);
		float z = std::cos(t / 1000.0);
		Light lightA = { { x * 2, 0, z * 2 }, { 1, 1, 1 }, 8 };
		Light lightB = { { x * -2, 0, z * -2 }, { 1, 1, 1 }, 8 };
		Transform lat = { { x * 2, 0, z * 2 }, IDR, IDS / 8 };
		Transform lbt = { { x * -2, 0, z * -2 }, IDR, IDS / 8 };

		r.beginBufferRender();
		r.renderMeshBuffered(sphere, st, cam);
		r.renderMeshBuffered(cube, bgt, cam);
		r.endBufferRender();

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		r.beginDeferredRender();
		r.renderDeferredLighting(cam, lightA);
		r.renderDeferredLighting(cam, lightB);
		r.endDeferredRender();

		r.transferDepthBuffer();
		r.renderMeshForward(lightSphere, lat, cam, lightShader);
		r.renderMeshForward(lightSphere, lbt, cam, lightShader);

		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}
