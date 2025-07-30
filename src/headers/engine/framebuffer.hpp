#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdexcept>
#include <vector>
#include <optional>
#include <GL/glew.h>

class FramebufferTexture {
	friend class Framebuffer;

	unsigned int id;
	GLenum formati, format, datatype, attachment;

	void resize(unsigned int, unsigned int);

  public:
	FramebufferTexture(GLenum, GLenum, GLenum, GLenum);
};

class Renderbuffer {
	friend class Framebuffer;

	unsigned int id;
	GLenum format, attachment;

	void resize(unsigned int, unsigned int);

  public:
	Renderbuffer(GLenum, GLenum);
};

class Framebuffer {
	unsigned int id;
	std::vector<FramebufferTexture> textures;
	std::vector<unsigned int> attachments;
	std::optional<Renderbuffer> rbo;

	std::runtime_error checkError(unsigned int);

  public:
	Framebuffer();
	Framebuffer(std::vector<FramebufferTexture>);
	Framebuffer(std::vector<FramebufferTexture>, Renderbuffer r);
	unsigned int getId();
	unsigned int w, h;
	void bind();
	void unbind();

	void resize(unsigned int, unsigned int);
	void clear(GLbitfield);

	void drawTo();
	void drawFrom();

	void blitTo(unsigned int, GLbitfield);
	void blitTo(Framebuffer, GLbitfield);
};

#endif
