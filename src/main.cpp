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
	Shader lightShader("shaders/buffer/v.glsl",
			"shaders/light/f.glsl");

	bufferShader.compile();
	deferredScreenShader.compile();
	lightShader.compile();

	FramebufferTexture gPosition(GL_RGBA16F, GL_RGBA, GL_FLOAT,
			GL_COLOR_ATTACHMENT0);
	FramebufferTexture gNormal(GL_RGBA16F, GL_RGBA, GL_FLOAT,
			GL_COLOR_ATTACHMENT1);
	FramebufferTexture gColor(GL_RGBA16F, GL_RGBA, GL_FLOAT,
			GL_COLOR_ATTACHMENT2);
	Renderbuffer gRbo(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
	Framebuffer gBuffer({ gPosition, gNormal, gColor }, gRbo);
	gBuffer.size(1920, 1080);

	Mesh deferredLightRadius = generateSphere(8);
	Mesh sphere = generateSphere(16);
	Mesh lightSphere = generateSphere(8);

	vector cPos = { 0, 0, 4 };
	vector cVel = { 0, 0, 0 };
	vector cAccel = { 0, 0, 0 };
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
		cDir.renormalize();
		cAccel = { 0, 0, 0 };
		if (input.s(12)) cAccel = -cVel;
		else {
			if (input.s(0)) cAccel -= cDir.rotate(vector{ 0, 0, 1 });
			if (input.s(1)) cAccel += cDir.rotate(vector{ 0, 0, 1 });
			if (input.s(2)) cAccel -= cDir.rotate(vector{ 1, 0, 0 });
			if (input.s(3)) cAccel += cDir.rotate(vector{ 1, 0, 0 });
			if (input.s(4)) cAccel -= cDir.rotate(vector{ 0, 1, 0 });
			if (input.s(5)) cAccel += cDir.rotate(vector{ 0, 1, 0 });
		}
		cVel += cAccel.normalize() / 256.0;
		cPos += cVel;
		Camera cam = { cPos, cDir,
			0.0625, 4096, M_PI / 2.0, 16.0 / 9.0 };

		Transform st = { { 16, 0, 0 }, IDR, IDS * 4, };
		Light light = { { 0, 0, 0 }, { 4, 4, 4 }, 32 };
		Transform lt = { { 0, 0, 0 }, IDR, IDS };

		gBuffer.bind();
		gBuffer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		gBuffer.drawTo();

		bufferShader.use();
		bufferShader.applyTransform(st);
		bufferShader.applyCamera(cam);
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
		deferredScreenShader.applyLight(light);
		deferredScreenShader.applyCamera(cam);
		deferredScreenShader.setVector("mTranslate", light.p);
		deferredScreenShader.setVector("mScale", IDS * light.r);
		deferredLightRadius.draw();

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		gBuffer.blitTo(0, GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		lightShader.use();
		lightShader.applyTransform(lt);
		lightShader.applyCamera(cam);
		lightSphere.draw();

		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}
