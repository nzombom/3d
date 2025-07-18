#ifndef RENDERER_H
#define RENDERER_H

#include "shader.hpp"
#include "mesh.hpp"
#include "meshUtils.hpp"

class Renderer {
	unsigned int fBuffer;
	unsigned int fImage;
	unsigned int frbo;

	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gColor;
	unsigned int grbo;

	Shader bufferShader;
	Shader deferredMtlShader;
	Shader renderShader;
	Mesh sphere = generateSphere(16);
	Mesh quad = simpleQuad();

	public:
	Renderer(Shader &, Shader &, Shader &);
	~Renderer();

	void beginGBuffer();
	void renderMeshBuffered(Mesh &, Transform, Camera);
	void endGBuffer();

	void beginRender();
	void endRender();
	void finalRenderToScreen();

	void beginDeferred();
	void renderDeferredLighting(Camera, Light);
	void endDeferred();

	void transferDepthBuffer();
	void renderMeshForward(Mesh &, Transform, Camera, Shader &);
};

#endif
