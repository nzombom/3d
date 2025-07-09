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
	{ INPUT_KEYBOARD, { .k = SDLK_I } },
	{ INPUT_KEYBOARD, { .k = SDLK_K } },
	{ INPUT_KEYBOARD, { .k = SDLK_J } },
	{ INPUT_KEYBOARD, { .k = SDLK_L } },
	{ INPUT_KEYBOARD, { .k = SDLK_U } },
	{ INPUT_KEYBOARD, { .k = SDLK_O } },
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
	SDL_Window * window = SDL_CreateWindow("3D", 1920, 1080,
			SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
	SDL_GL_CreateContext(window);
	glewInit();
	glViewport(0, 0, 1920, 1080);

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

	Mesh sphere = generateSphere(16);
	Mesh lightSphere = generateSphere(8);

	vector cPos = { 0, 0, 4 };
	quat cDir = IDR;

	unsigned int prev = 0;
	while (!quit) {
		event();
		unsigned int t = SDL_GetTicks();
		unsigned int dt = t - prev;
		prev = t;

		float cost = std::cos(M_PI / 256.0);
		float sint = std::sin(M_PI / 256.0);
		if (input.s(6)) cDir *= quat{ cost, vector{ 1, 0, 0 } * sint };
		if (input.s(7)) cDir *= quat{ cost, vector{ -1, 0, 0 } * sint };
		if (input.s(8)) cDir *= quat{ cost, vector{ 0, 1, 0 } * sint };
		if (input.s(9)) cDir *= quat{ cost, vector{ 0, -1, 0 } * sint };
		if (input.s(10)) cDir *= quat{ cost, vector{ 0, 0, 1 } * sint };
		if (input.s(11)) cDir *= quat{ cost, vector{ 0, 0, -1 } * sint };
		if (input.s(0)) cPos -= cDir.rotate(vector{ 0, 0, 0.25 });
		if (input.s(1)) cPos += cDir.rotate(vector{ 0, 0, 0.25 });
		if (input.s(2)) cPos -= cDir.rotate(vector{ 0.25, 0, 0 });
		if (input.s(3)) cPos += cDir.rotate(vector{ 0.25, 0, 0 });
		if (input.s(4)) cPos -= cDir.rotate(vector{ 0, 0.25, 0 });
		if (input.s(5)) cPos += cDir.rotate(vector{ 0, 0.25, 0 });
		cDir.renormalize();
		Camera cam = { cPos, cDir,
			1, 64, M_PI / 2.0, 16.0 / 9.0 };

		Transform st = { { 16, 0, 0 }, IDR, IDS * 4, };
		Light light = { { 0, 0, 0 }, { 4, 4, 4 }, 32 };
		Transform lt = { { 0, 0, 0 }, IDR, IDS };

		r.beginBufferRender();
		r.renderMeshBuffered(sphere, st, cam);
		r.endBufferRender();

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		r.beginDeferredRender();
		r.renderDeferredLighting(cam, light);
		r.endDeferredRender();

		r.transferDepthBuffer();
		r.renderMeshForward(lightSphere, lt, cam, lightShader);

		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}
