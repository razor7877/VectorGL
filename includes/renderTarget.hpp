#pragma once

#include <utilities/glad.h>
#include <glm/glm.hpp>

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
	GLuint depthbuffer{};

	/// <summary>
	/// The OpenGL handle for the render texture
	/// </summary>
	GLuint renderTexture{};

	/// <summary>
	/// The OpenGL handle for the draw buffer
	/// </summary>
	GLenum drawBuffers[1]{};

	/// <summary>
	/// The size of the render target in pixels
	/// </summary>
	glm::vec2 size;

	/// <summary>
	/// Whether the render target uses multisampling
	/// </summary>
	bool isMultiSampled;
	
	RenderTarget();

	/// <summary>
	/// Creates a new render target
	/// </summary>
	/// <param name="size">The size of the render target in pixels</param>
	/// <param name="useMultiSampling">Whether the render target is multisampled or not</param>
	RenderTarget(glm::vec2 size, bool useMultiSampling);

	/// <summary>
	/// Binds the render target
	/// </summary>
	void bind();

	/// <summary>
	/// Unbinds the render target
	/// </summary>
	void unbind();

	/// <summary>
	/// Resizes the render target
	/// </summary>
	/// <param name="newSize">The new size of the render target</param>
	void resize(glm::vec2 newSize);

private:
	void createRenderTarget();
	void createMultiSampledRenderTarget();
};