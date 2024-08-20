#pragma once

#include <utilities/glad.h>
#include <glm/glm.hpp>

enum class TargetType
{
	TEXTURE_2D,
	TEXTURE_2D_MULTISAMPLE,
	TEXTURE_CUBEMAP,
};

/// <summary>
/// Represents a render target that can be drawn into
/// </summary>
class RenderTarget
{
public:
	/// <summary>
	/// The OpenGL handle for the framebuffer
	/// </summary>
	GLuint framebuffer{};

	/// <summary>
	/// The OpenGL handle for the depth buffer
	/// </summary>
	GLuint depthStencilBuffer{};

	/// <summary>
	/// The OpenGL handle for the render texture
	/// </summary>
	GLuint renderTexture{};

	/// <summary>
	/// The OpenGL handle for the draw buffer
	/// </summary>
	GLenum drawBuffers[1]{};

	/// <summary>
	/// The pixel format of the framebuffer
	/// </summary>
	GLenum format = GL_RGB;

	/// <summary>
	/// The size of the render target in pixels
	/// </summary>
	glm::vec2 size = glm::vec2(0);

	/// <summary>
	/// The type of the target texture attached to the framebuffer
	/// </summary>
	TargetType targetTextureType = TargetType::TEXTURE_2D;

	RenderTarget();

	/// <summary>
	/// Creates a new render target
	/// </summary>
	/// <param name="size">The size of the render target in pixels</param>
	/// <param name="useMultiSampling">Whether the render target is multisampled or not</param>
	RenderTarget(TargetType targetTextureType, glm::vec2 size, GLenum format = GL_RGB);

	~RenderTarget();

	/// <summary>
	/// Binds the render target
	/// </summary>
	void bind();

	/// <summary>
	/// Unbinds the render target
	/// </summary>
	void unbind();

	/// <summary>
	/// Clears the render target buffers
	/// </summary>
	void clear();

	/// <summary>
	/// Resizes the render target
	/// </summary>
	/// <param name="newSize">The new size of the render target</param>
	void resize(glm::vec2 newSize);

private:
	void attachTexture(TargetType targetTextureType, glm::vec2 size);
};