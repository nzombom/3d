#ifndef RENDERER_H
#define RENDERER_H

#include "shader.hpp"
#include "mesh.hpp"
#include "meshUtils.hpp"

class Renderer {
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gColor;
	unsigned int rbo;
	Shader bufferShader;
	Shader deferredMtlShader;
	Mesh sphere = generateSphere(16);

	public:
	Renderer(Shader &, Shader &);
	~Renderer();

	void beginBufferRender();
	void renderMeshBuffered(Mesh &, Transform, Camera);
	void endBufferRender();

	void beginDeferredRender();
	void renderDeferredLighting(Camera, Light);
	void endDeferredRender();

	void transferDepthBuffer();
	void renderMeshForward(Mesh &, Transform, Camera, Shader &);
};

#endif
