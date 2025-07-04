#include "renderTarget.hpp"
#include "logger.hpp"
#include "renderer.hpp"

RenderTarget::RenderTarget() = default;

RenderTarget::RenderTarget(TargetType targetTextureType, glm::vec2 size, GLenum format)
{
	this->size = size;
	this->format = format;
	this->targetTextureType = targetTextureType;

	this->attachTexture(targetTextureType, size);
	RenderTarget::unbind();
}

RenderTarget::~RenderTarget()
{
	glDeleteFramebuffers(1, &this->framebuffer);
	glDeleteRenderbuffers(1, &this->depthStencilBuffer);
	glDeleteTextures(1, &this->renderTexture);
	glDeleteTextures(1, &this->gPosition);
	glDeleteTextures(1, &this->gNormal);
	glDeleteTextures(1, &this->gAlbedo);
}

void RenderTarget::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	glViewport(0, 0, this->size.x, this->size.y);
}

void RenderTarget::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::clear() const
{
	if (this->targetTextureType == TargetType::TEXTURE_CUBEMAP || this->targetTextureType == TargetType::TEXTURE_DEPTH || this->targetTextureType == TargetType::TEXTURE_DEPTH_3D)
		glClear(GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void RenderTarget::resize(glm::vec2 newSize)
{
	if (this->size.x == newSize.x && this->size.y == newSize.y)
		return;

	// Save the new size
	this->size = newSize;

	glDeleteFramebuffers(1, &this->framebuffer);
	glDeleteRenderbuffers(1, &this->depthStencilBuffer);
	glDeleteTextures(1, &this->renderTexture);
	glDeleteTextures(1, &this->gPosition);
	glDeleteTextures(1, &this->gNormal);
	glDeleteTextures(1, &this->gAlbedo);

	this->framebuffer = 0;
	this->depthStencilBuffer = 0;
	this->renderTexture = 0;
	this->gPosition = 0;
	this->gNormal = 0;
	this->gAlbedo = 0;

	this->attachTexture(this->targetTextureType, this->size);
	glViewport(0, 0, this->size.x, this->size.y);
}

void RenderTarget::attachTexture(TargetType targetTextureType, glm::vec2 size)
{
	glGenFramebuffers(1, &this->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

	switch (this->targetTextureType)
	{
		case TargetType::TEXTURE_2D:
		{
			// Depth buffer for the FBO
			glGenRenderbuffers(1, &this->depthStencilBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->size.x, this->size.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthStencilBuffer);

			// Render texture
			glGenTextures(1, &this->renderTexture);
			glBindTexture(GL_TEXTURE_2D, this->renderTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, this->format, this->size.x, this->size.y, 0, this->format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			// Set texture as color attachment 0
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->renderTexture, 0);

			this->drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);
			break;
		}

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
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->renderTexture, 0);

			this->drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);
			break;
		}
		
		case TargetType::TEXTURE_CUBEMAP:
		{
			// Depth buffer for the FBO
			glGenRenderbuffers(1, &this->depthStencilBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->size.x, this->size.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthStencilBuffer);
			break;
		}

		case TargetType::TEXTURE_DEPTH:
		{
			glGenTextures(1, &this->renderTexture);
			glBindTexture(GL_TEXTURE_2D, this->renderTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, this->format, this->size.x, this->size.y, 0, this->format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->renderTexture, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			break;
		}

		case TargetType::TEXTURE_DEPTH_3D:
		{
			glGenTextures(1, &this->renderTexture);
			glBindTexture(GL_TEXTURE_2D_ARRAY, this->renderTexture);
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, this->size.x, this->size.y, Renderer::SHADOW_CASCADE_LEVELS + 1, 0, this->format, GL_FLOAT, nullptr);

			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->renderTexture, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			break;
		}

		case TargetType::G_BUFFER:
		{
			// Depth buffer for the FBO
			glGenRenderbuffers(1, &this->depthStencilBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->size.x, this->size.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthStencilBuffer);

			// G-buffer position texture
			glGenTextures(1, &this->gPosition);
			glBindTexture(GL_TEXTURE_2D, this->gPosition);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->size.x, this->size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->gPosition, 0);

			// G-buffer normal texture
			glGenTextures(1, &this->gNormal);
			glBindTexture(GL_TEXTURE_2D, this->gNormal);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->size.x, this->size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, this->gNormal, 0);

			// G-buffer albedo texture
			glGenTextures(1, &this->gAlbedo);
			glBindTexture(GL_TEXTURE_2D, this->gAlbedo);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->size.x, this->size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, this->gAlbedo, 0);

			this->drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
			this->drawBuffers[1] = { GL_COLOR_ATTACHMENT1 };
			this->drawBuffers[2] = { GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(3, drawBuffers);
			break;
		}

		case TargetType::TEXTURE_RED:
			glGenTextures(1, &this->renderTexture);
			glBindTexture(GL_TEXTURE_2D, this->renderTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->size.x, this->size.y, 0, GL_RED, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->renderTexture, 0);
			break;
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Logger::logError("Error while attempting to create framebuffer!", "renderTarget.cpp");
}
