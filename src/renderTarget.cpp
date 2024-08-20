#include "renderTarget.hpp"
#include "logger.hpp"

RenderTarget::RenderTarget()
{

}

RenderTarget::RenderTarget(TargetType targetTextureType, glm::vec2 size, GLenum format)
{
	this->size = size;
	this->format = format;
	this->targetTextureType = targetTextureType;

	this->attachTexture(targetTextureType, size);
}

RenderTarget::~RenderTarget()
{
	//glDeleteFramebuffers(1, &this->framebuffer);
}

void RenderTarget::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	glViewport(0, 0, this->size.x, this->size.y);
}

void RenderTarget::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::clear()
{
	if (this->targetTextureType == TargetType::TEXTURE_CUBEMAP || this->targetTextureType == TargetType::TEXTURE_DEPTH)
		glClear(GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderTarget::resize(glm::vec2 newSize)
{
	if (this->size.x == newSize.x && this->size.y == newSize.y)
		return;

	this->size = newSize;

	switch (this->targetTextureType)
	{
		case TargetType::TEXTURE_2D:
			// Bind the texture
			glBindTexture(GL_TEXTURE_2D, this->renderTexture);
			// Create a 2D texture
			glTexImage2D(GL_TEXTURE_2D, 0, this->format, this->size.x, this->size.y, 0, this->format, GL_UNSIGNED_BYTE, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Attach it to the FBO
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->renderTexture, 0);

			glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->size.x, this->size.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthStencilBuffer);
			break;

		case TargetType::TEXTURE_2D_MULTISAMPLE:
			// Bind the texture
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->renderTexture);
			// Create a 2D multisampled texture
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, this->format, this->size.x, this->size.y, GL_TRUE);

			// Attach it to the FBO
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, this->renderTexture, 0);

			glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, this->size.x, this->size.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthStencilBuffer);
			break;

		case TargetType::TEXTURE_CUBEMAP:
			glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->size.x, this->size.y);
			break;
	}

	glViewport(0, 0, this->size.x, this->size.y);
}

void RenderTarget::attachTexture(TargetType targetTextureType, glm::vec2 size)
{
	glGenFramebuffers(1, &this->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

	switch (this->targetTextureType)
	{
		case TargetType::TEXTURE_2D:
			// Depth buffer for the FBO
			glGenRenderbuffers(1, &this->depthStencilBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->size.x, this->size.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthStencilBuffer);

			// Render texture
			glGenTextures(1, &this->renderTexture);
			glBindTexture(GL_TEXTURE_2D, this->renderTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, this->format, this->size.x, this->size.y, 0, this->format, GL_UNSIGNED_BYTE, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			// Set texture as color attachment 0
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->renderTexture, 0);

			this->drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);
			break;

		case TargetType::TEXTURE_2D_MULTISAMPLE:
		{
			// Depth buffer for the FBO
			glGenRenderbuffers(1, &this->depthStencilBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, this->size.x, this->size.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthStencilBuffer);

			// Render texture
			glGenTextures(1, &this->renderTexture);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->renderTexture);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, this->format, this->size.x, this->size.y, GL_TRUE);

			// Set texture as color attachment 0
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->renderTexture, 0);

			this->drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);
			break;
		}
			
		case TargetType::TEXTURE_CUBEMAP:
			// Depth buffer for the FBO
			glGenRenderbuffers(1, &this->depthStencilBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->size.x, this->size.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthStencilBuffer);
			break;

		case TargetType::TEXTURE_DEPTH:
			glGenTextures(1, &this->renderTexture);
			glBindTexture(GL_TEXTURE_2D, this->renderTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, this->format, this->size.x, this->size.y, 0, this->format, GL_UNSIGNED_BYTE, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->renderTexture, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			break;
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Logger::logError("Error while attempting to create framebuffer!", "renderTarget.cpp");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
