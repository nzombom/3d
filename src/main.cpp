#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "math.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "meshUtils.hpp"
#include "framebuffer.hpp"
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
	{ INPUT_KEYBOARD, { .k = SDLK_SPACE } },
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

	Shader bufferShader("shaders/buffer/v.glsl",
			"shaders/buffer/f.glsl");
	Shader deferredScreenShader("shaders/deferred/v.glsl",
			"shaders/deferred/f.glsl");
	Shader lightShader("shaders/light/v.glsl",
			"shaders/light/f.glsl");

	bufferShader.compile();
	deferredScreenShader.compile();
	lightShader.compile();

	FramebufferTexture gPosition(GL_RGBA16F, GL_RGBA, GL_FLOAT,
			GL_COLOR_ATTACHMENT0);
	FramebufferTexture gNormal(GL_RGBA16F, GL_RGBA, GL_FLOAT,
			GL_COLOR_ATTACHMENT1);
	FramebufferTexture gColor(GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT2);
	Renderbuffer gRbo(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
	Framebuffer gBuffer({ gPosition, gNormal, gColor }, gRbo);
	gBuffer.size(1920, 1080);

	Mesh deferredLightRadius = generateSphere(8);
	Mesh sphere = generateSphere(32);
	Mesh lightSphere = generateSphere(32);

	vector cPos = { 0, 0, 8 };
	vector cVel = { 0, 0, 0 };
	vector cAccel = { 0, 0, 0 };
	quat cDir = IDR;

	unsigned int prev = 0;
	while (!quit) {
		event();
		unsigned int t = SDL_GetTicks();
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
			cAccel += cDir.rotate(vector{ 0, 0, 1 } * input.getAxis(0, 1));
			cAccel += cDir.rotate(vector{ 1, 0, 0 } * input.getAxis(2, 3));
			cAccel += cDir.rotate(vector{ 0, 1, 0 } * input.getAxis(4, 5));
		}
		cVel += cAccel.normalize() / 256.0;
		cPos += cVel;
		Camera cam = { cPos, cDir,
			1.0 / 64.0, 1024, M_PI / 2.0, 16.0 / 9.0 };

		Transform st = { { 16, 0, 0 }, IDR, IDS };
		Light sun = { { 0, 0, 0 }, { 4, 4, 4 }, 32 };
		Transform sunt = { { 0, 0, 0 }, IDR, IDS * 2 };

		gBuffer.bind();
		gBuffer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		gBuffer.drawTo();

		bufferShader.use();
		bufferShader.applyCamera(cam);
		bufferShader.applyTransform(st);
		sphere.draw();

		gBuffer.unbind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		deferredScreenShader.use();
		gBuffer.drawFrom();
		deferredScreenShader.applyCamera(cam);
		deferredScreenShader.applyLight(sun);
		deferredScreenShader.setVector("mTranslate", sun.p);
		deferredScreenShader.setVector("mScale", IDS * sun.r);
		deferredLightRadius.draw();

		glDisable(GL_BLEND);
		glCullFace(GL_BACK);
		gBuffer.blitTo(0, GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		lightShader.use();
		lightShader.applyTransform(sunt);
		lightShader.applyCamera(cam);
		lightSphere.draw();

		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}
