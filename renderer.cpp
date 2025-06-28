#include <stdexcept>
#include <GL/glew.h>

#include "renderer.hpp"
#include "shader.hpp"
#include "mesh.hpp"

Renderer::Renderer(Shader &b, Shader &l) {
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
			960, 540, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
			960, 540, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
			GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gColor);
	glBindTexture(GL_TEXTURE_2D, gColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
			960, 540, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
			GL_TEXTURE_2D, gColor, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 960, 540);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
			!= GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("\e[1;31merror with framebuffer\e[m");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bufferShader = b;
	deferredMtlShader = l;
}
Renderer::~Renderer() {
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gColor);
	glDeleteFramebuffers(1, &gBuffer);
}

void Renderer::beginBufferRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	unsigned int attachments[3] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
	};
	glDrawBuffers(3, attachments);
}
void Renderer::renderMeshBuffered(Mesh &m, Transform t, Camera c) {
	bufferShader.use();
	bufferShader.setVector("mTranslate", t.p);
	bufferShader.setQuat("mRotate", t.r);
	bufferShader.setVector("mScale", t.s);
	bufferShader.applyCamera(c);
	m.draw();
}
void Renderer::endBufferRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::beginDeferredRender() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}
void Renderer::renderDeferredLighting(Camera c, Light l) {
	deferredMtlShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gColor);
	deferredMtlShader.applyLight(l);
	deferredMtlShader.applyCamera(c);
	deferredMtlShader.setVector("mTranslate", l.p);
	deferredMtlShader.setVector("mScale", { l.r, l.r, l.r });
	sphere.draw();
}
void Renderer::endDeferredRender() {
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void Renderer::transferDepthBuffer() {
	glBlitNamedFramebuffer(gBuffer, 0,
			0, 0, 960, 540, 0, 0, 960, 540,
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glEnable(GL_DEPTH_TEST);
}
void Renderer::renderMeshForward(Mesh &m, Transform t, Camera c, Shader &s) {
	s.use();
	s.setVector("mTranslate", t.p);
	s.setQuat("mRotate", t.r);
	s.setVector("mScale", t.s);
	s.applyCamera(c);
	m.draw();
}
