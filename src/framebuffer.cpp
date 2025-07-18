#include <stdexcept>
#include <vector>
#include <GL/glew.h>

#include "framebuffer.hpp"

FramebufferTexture::FramebufferTexture(GLenum fi, GLenum f, GLenum t, GLenum a)
	: formati(fi), format(f), datatype(t), attachment(a) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, formati, 1, 1, 0, format, datatype, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void FramebufferTexture::size(unsigned int w, unsigned int h) {
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, formati, w, h, 0, format, datatype, NULL);
}

Renderbuffer::Renderbuffer(GLenum f, GLenum a)
	: format(f), attachment(a) {
	glGenRenderbuffers(1, &id);
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	glRenderbufferStorage(GL_RENDERBUFFER, format, 1, 1);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderbuffer::size(unsigned int w, unsigned int h) {
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

std::runtime_error Framebuffer::checkError(unsigned int status) {
	switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			return std::runtime_error("\e[1;31merror with framebuffer: \e[m"
					"incomplete attachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			return std::runtime_error("\e[1;31merror with framebuffer: \e[m"
					"missing attachment");
			break;
		default:
			return std::runtime_error("\e[1;31merror with framebuffer\e[m");
	}
}

Framebuffer::Framebuffer(std::vector<FramebufferTexture> t)
	: textures(t) {
	glGenFramebuffers(1, &id);
	bind();

	for (unsigned int i = 0; i < textures.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, textures.at(i).id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, textures.at(i).attachment,
				GL_TEXTURE_2D, textures.at(i).id, 0);
		attachments.push_back(textures.at(i).attachment);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) throw checkError(status);
	unbind();
}
Framebuffer::Framebuffer(std::vector<FramebufferTexture> t, Renderbuffer r)
	: textures(t), rbo(r) {
	glGenFramebuffers(1, &id);
	bind();

	for (unsigned int i = 0; i < textures.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, textures.at(i).id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, textures.at(i).attachment,
				GL_TEXTURE_2D, textures.at(i).id, 0);
		attachments.push_back(textures.at(i).attachment);
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, rbo->id);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) throw checkError(status);

	unbind();
}
unsigned int Framebuffer::getId() {
	return id;
}
void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}
void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::size(unsigned int _w, unsigned int _h) {
	w = _w;
	h = _h;

	for (unsigned int i = 0; i < textures.size(); i++) {
		textures.at(i).size(w, h);
	}
	if (rbo.has_value()) rbo->size(w, h);
}
void Framebuffer::clear(GLbitfield bits) {
	glClear(bits);
}

void Framebuffer::drawTo() {
	glDrawBuffers(3, &attachments[0]);
}
void Framebuffer::drawFrom() {
	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures.at(i).id);
	}
}

void Framebuffer::blitTo(unsigned int f, GLbitfield bits) {
	glBlitNamedFramebuffer(id, f,
			0, 0, w, h, 0, 0, w, h,
			bits, GL_NEAREST);
}
void Framebuffer::blitTo(Framebuffer f, GLbitfield bits) {
	glBlitNamedFramebuffer(id, f.getId(),
			0, 0, w, h, 0, 0, f.w, f.h,
			bits, GL_NEAREST);
}
