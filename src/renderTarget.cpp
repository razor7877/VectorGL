#include "renderTarget.hpp"
#include "logger.hpp"

RenderTarget::RenderTarget()
{

}

RenderTarget::RenderTarget(glm::vec2 size, bool useMultiSampling)
{
	this->size = size;
	this->isMultiSampled = useMultiSampling;
	
	if (useMultiSampling)
		this->createMultiSampledRenderTarget();
	else
		this->createRenderTarget();
}

void RenderTarget::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	glViewport(0, 0, this->size.x, this->size.y);

	// Clear the buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTarget::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::resize(glm::vec2 newSize)
{
	if (this->size.x == newSize.x && this->size.y == newSize.y)
		return;

	this->size = newSize;

	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);

	if (this->isMultiSampled)
	{
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->renderTexture);
		// Create a 2D multisampled texture
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, this->size.x, this->size.y, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Attach it to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, this->renderTexture, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, this->depthbuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, this->size.x, this->size.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthbuffer);
	}
	else
	{
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, this->renderTexture);
		// Create a 2D texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->size.x, this->size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Attach it to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->renderTexture, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, this->depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->size.x, this->size.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthbuffer);
	}
}

void RenderTarget::createRenderTarget()
{
	glGenFramebuffers(1, &this->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Render texture for the FBO
	glGenTextures(1, &this->renderTexture);
	glBindTexture(GL_TEXTURE_2D, this->renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->size.x, this->size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Depth buffer for the FBO
	glGenRenderbuffers(1, &this->depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->size.x, this->size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthbuffer);

	// Set texture as color attachment 0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->renderTexture, 0);

	this->drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Logger::logError("Error while attempting to create framebuffer!");
}

void RenderTarget::createMultiSampledRenderTarget()
{
	glGenFramebuffers(1, &this->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Render texture for the FBO
	glGenTextures(1, &this->renderTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->renderTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, this->size.x, this->size.y, GL_TRUE);

	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Depth buffer for the FBO
	glGenRenderbuffers(1, &this->depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, this->size.x, this->size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthbuffer);

	// Set texture as color attachment 0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->renderTexture, 0);

	this->drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Logger::logError("Error while attempting to create multisampled framebuffer!");
}
