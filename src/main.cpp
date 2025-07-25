#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "engine/math.hpp"
#include "engine/shader.hpp"
#include "engine/mesh.hpp"
#include "engine/meshUtils.hpp"
#include "engine/framebuffer.hpp"
#include "engine/input.hpp"
#include "celestial.hpp"
#include "planet.hpp"

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
	{ INPUT_KEYBOARD, { .k = SDLK_SPACE } },
};
InputState input = InputState(controls);

void event() {
	SDL_Event e;
	while (SDL_PollEvent(&e))
		switch (e.type) {
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

	Shader shaderSolid("shaders/solid/v.glsl",
		"shaders/solid/f.glsl");
	Shader shaderSun("shaders/light/v.glsl",
		"shaders/light/f.glsl");

	shaderSolid.compile();
	shaderSun.compile();

	Mesh sphere = generateSphere(32);

	Planet sun("The Sun", { 0, 0, 0 }, { 0, 0, 0 }, 32, 16, sphere);
	Planet earth("Earth", { 256, 0, 0 }, { 0, 0, 8 }, 16, 1, sphere);
	Planet anti("Anti-Earth", { -256, 0, 0 }, { 0, 0, -8 }, 16, 1, sphere);
	std::vector<Celestial *> celestials{ &sun, &earth, &anti };

	vector cPos = { 0, 0, 256 };
	vector cVel = { 0, 0, 0 };
	vector cAccel = { 0, 0, 0 };
	quat cDir = IDR;

	const float baseAccel = 8.0;

	unsigned long long prev = 0;
	while (!quit) {
		event();
		unsigned long long t = SDL_GetTicks();
		unsigned int dt = t - prev;
		prev = t;

		float angle = M_PI / 256.0;
		cDir *= qFromAA(vector{ -1, 0, 0 }, input.getAxis(6, 7) * angle);
		cDir *= qFromAA(vector{ 0, -1, 0 }, input.getAxis(8, 9) * angle);
		cDir *= qFromAA(vector{ 0, 0, -1 }, input.getAxis(10, 11) * angle);
		cDir.renormalize();
		cAccel = { 0, 0, 0 };
		if (input.s(12)) cAccel = -cVel;
		else {
			cAccel += cDir.rotate(vector{ 0, 0, 1 }) * input.getAxis(0, 1);
			cAccel += cDir.rotate(vector{ 1, 0, 0 }) * input.getAxis(2, 3);
			cAccel += cDir.rotate(vector{ 0, 1, 0 }) * input.getAxis(4, 5);
		}
		cVel += cAccel.normalize() * baseAccel * dt / 1000.0;
		cPos += cVel * dt / 1000.0;
		Camera cam = { cPos, cDir, 1.0 / 16.0, 8192, M_PI / 2.0, 16.0 / 9.0 };

		for (unsigned int i = 0; i < celestials.size(); i++) {
			celestials.at(i)->updateVel(celestials, dt);
		}
		for (unsigned int i = 0; i < celestials.size(); i++) {
			celestials.at(i)->updatePos(celestials, dt);
		}

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Light sun = { celestials.at(0)->getPos(), { 2, 2, 2 }, 1024 };
		for (unsigned int i = 1; i < celestials.size(); i++) {
			shaderSolid.use();
			shaderSolid.setVector("view.translate", -cam.p);
			shaderSolid.setQuat("view.rotate", cam.r.conj());
			shaderSolid.setMatrix("view.projection", cam.projectionMatrix());
			celestials.at(i)->draw(shaderSolid);
		}
		shaderSun.use();
		shaderSun.setVector("view.translate", -cam.p);
		shaderSun.setQuat("view.rotate", cam.r.conj());
		shaderSun.setMatrix("view.projection", cam.projectionMatrix());
		celestials.at(0)->draw(shaderSun);

		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}
