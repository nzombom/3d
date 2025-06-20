#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "math.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "meshUtils.hpp"
#include "renderer.hpp"

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

	Shader bufferShader("shaders/bufferv.glsl",
			"shaders/bufferf.glsl");
	Shader deferredScreenShader("shaders/deferredv.glsl",
			"shaders/deferredf.glsl");
	Renderer r(bufferShader, deferredScreenShader);
	Shader lightShader("shaders/bufferv.glsl",
			"shaders/lightf.glsl");

	Camera cam = { { 0, 0, 4 }, { 1, { 0, 0, 0 } },
			1, 16, M_PI / 2.0, 16.0 / 9.0 };

	Mesh sphere = generateSphere(32);
	Mesh lightSphere = generateSphere(8);

	while (!quit) {
		event();
		unsigned int t = SDL_GetTicks();

		float y = std::sin(t / 1000.0);
		float qr = std::sin(t / 2.0 / 1000.0);
		float qw = std::cos(t / 2.0 / 1000.0);
		Transform st = {
			{ 0, y * 2, 0 },
			{ qw, { 0, qr, 0 } },
			{ 1, 1, 1 },
		};
		float x = std::sin(t / 1000.0);
		float z = std::cos(t / 1000.0);
		Light lightA = { { x * 2, 0, z * 2 }, { 1, 1, 1 }, 4 };
		Light lightB = { { x * -2, 0, z * -2 }, { 1, 1, 1 }, 4 };
		Transform lat = { { x * 2, 0, z * 2 }, IDR, IDS / 8 };
		Transform lbt = { { x * -2, 0, z * -2 }, IDR, IDS / 8 };

		r.beginBufferRender();
		r.renderMeshBuffered(sphere, st, cam);
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
