#include <iostream>
#include <cmath>
#include <string>
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

static bool quit = false;
static InputState input({
	{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_SPACE } },
}, {
	{
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_I } },
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_K } },
	},
	{
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_J } },
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_L } },
	},
	{
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_U } },
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_O } },
	},
	{
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_W } },
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_S } },
	},
	{
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_A } },
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_D } },
	},
	{
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_Q } },
		{ INPUT_TYPE::keyboard, { .k = SDL_SCANCODE_E } },
	},
});

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
	Shader shaderSun("shaders/sun/v.glsl",
		"shaders/sun/f.glsl");

	shaderSolid.compile();
	shaderSun.compile();

	Mesh sphere = generateSphere(32);

	Planet sun("The Sun", { 0, 0, 0 }, { 0, 0, 0 }, 64, 32, sphere);
	Planet earth("Earth", { 256, 0, 0 }, { 0, 0, 16 }, 8, 8, sphere);
	Planet anti("Anti-Earth", { -256, 0, 0 }, { 0, 0, -16 }, 8, 8, sphere);
	std::vector<Celestial *> celestials{ &sun, &earth, &anti };

	vector cPos = { 0, 0, 256 };
	vector cVel = { 0, 0, 0 };
	vector cAccel = { 0, 0, 0 };
	quat cDir = identity::rotation;

	const float baseAccel = 8;

	unsigned long long prev = 0;
	unsigned long frameNumber = 0;
	while (!quit) {
		event();
		unsigned long long t = SDL_GetTicksNS();
		double dt = (t - prev) / 1.0e9;
		prev = t;
		frameNumber++;

		float angle = M_PI / 4.0 * dt;
		cDir *= quat::fromAA(vector{ -1, 0, 0 }, input.getAxis(0) * angle);
		cDir *= quat::fromAA(vector{ 0, -1, 0 }, input.getAxis(1) * angle);
		cDir *= quat::fromAA(vector{ 0, 0, -1 }, input.getAxis(2) * angle);
		cDir.renormalize();
		cAccel = { 0, 0, 0 };
		if (input.getControl(0)) cAccel = -cVel;
		else {
			cAccel += cDir.rotate(vector{ 0, 0, 1 }) * input.getAxis(3);
			cAccel += cDir.rotate(vector{ 1, 0, 0 }) * input.getAxis(4);
			cAccel += cDir.rotate(vector{ 0, 1, 0 }) * input.getAxis(5);
		}
		cVel += cAccel.normalize() * baseAccel * dt;
		for (unsigned int i = 0; i < celestials.size(); i++) {
			Celestial * cel = celestials.at(i);
			vector dir = cel->getPos() - cPos;
			float gravAccel = G * cel->getMass() / dir.slen();
			cVel += dir.normalize() * gravAccel * dt;
		}
		cPos += cVel * dt;
		Camera cam = { cPos, cDir, 1.0 / 16.0, 8192, M_PI / 2.0, 16.0 / 9.0 };

		for (unsigned int i = 0; i < celestials.size(); i++) {
			celestials.at(i)->updateVel(celestials, dt);
		}
		for (unsigned int i = 0; i < celestials.size(); i++) {
			celestials.at(i)->updatePos(celestials, dt);
		}

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Light sun = { celestials.at(0)->getPos(), { 32768, 32768, 32768 } };
		for (unsigned int i = 1; i < celestials.size(); i++) {
			shaderSolid.use();
			shaderSolid.setVector("view.translate", -cam.p);
			shaderSolid.setQuat("view.rotate", cam.r.conj());
			shaderSolid.setMatrix("view.projection", cam.projectionMatrix());
			shaderSolid.setVector("light.pos", sun.p);
			shaderSolid.setVector("light.intensity", sun.c);
			shaderSolid.setVector("mtl.color", { 1, 0.5, 0.5 });
			shaderSolid.setFloat("mtl.specular", 0.25);
			celestials.at(i)->draw(shaderSolid);
		}
		shaderSun.use();
		shaderSun.setVector("view.translate", -cam.p);
		shaderSun.setQuat("view.rotate", cam.r.conj());
		shaderSun.setMatrix("view.projection", cam.projectionMatrix());
		celestials.at(0)->draw(shaderSun);

		SDL_GL_SwapWindow(window);

		if (frameNumber % 120 == 0) std::cout << "last frame " << 1.0 / dt
				<< " fps" << std::endl;
	}

	SDL_Quit();
	return 0;
}
