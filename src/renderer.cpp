#include <stdexcept>
#include <GL/glew.h>

#include "renderer.hpp"
#include "shader.hpp"
#include "mesh.hpp"

Renderer::Renderer(Shader & b, Shader & l, Shader & r)
	: bufferShader(b), deferredMtlShader(l), renderShader(r) {
	glGenFramebuffers(1, &fBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, fBuffer);

	glGenTextures(1, &fImage);
	glBindTexture(GL_TEXTURE_2D, fImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
			1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, fImage, 0);

	glGenRenderbuffers(1, &frbo);
	glBindRenderbuffer(GL_RENDERBUFFER, frbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, frbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
			!= GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("\e[1;31merror with framebuffer\e[m");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
			1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
			1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
			GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gColor);
	glBindTexture(GL_TEXTURE_2D, gColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
			1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
			GL_TEXTURE_2D, gColor, 0);

	glGenRenderbuffers(1, &grbo);
	glBindRenderbuffer(GL_RENDERBUFFER, grbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, grbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
			!= GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("\e[1;31merror with framebuffer\e[m");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Renderer::~Renderer() {
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gColor);
	glDeleteFramebuffers(1, &gBuffer);
}

void Renderer::beginGBuffer() {
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
void Renderer::endGBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::beginRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, fBuffer);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}
void Renderer::endRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::finalRenderToScreen() {
	renderShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fImage);
	quad.draw();
}

void Renderer::beginDeferred() {
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
void Renderer::endDeferred() {
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void Renderer::transferDepthBuffer() {
	glBlitNamedFramebuffer(gBuffer, fBuffer,
			0, 0, 1920, 1080, 0, 0, 1920, 1080,
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
