#pragma once

#include <utilities/glad.h>
#include <glm/glm.hpp>

enum class TargetType
{
	TEXTURE_2D, // Depth + stencil + one color attachment
	TEXTURE_2D_MULTISAMPLE, // Depth + stencil + one color attachment with MSAA x4
	TEXTURE_CUBEMAP, // Color only?
	TEXTURE_DEPTH, // Depth only
	TEXTURE_DEPTH_3D, // 3D Depth only, for cascaded shadow maps
	G_BUFFER, // 3 color attachments + depth
	TEXTURE_RED, // 1 color attachment, single channel
};

// TODO : Use inheritance 

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
	/// The OpenGL handle for the G-buffer position texture
	/// </summary>
	GLuint gPosition{};

	/// <summary>
	/// The OpenGL handle for the G-buffer normal texture
	/// </summary>
	GLuint gNormal{};

	/// <summary>
	/// The OpenGL handle for the G-buffer albedo texture
	/// </summary>
	GLuint gAlbedo{};

	/// <summary>
	/// The OpenGL handle for the draw buffers
	/// </summary>
	GLenum drawBuffers[4]{};

	/// <summary>
	/// The pixel format of the framebuffer
	/// </summary>
	GLenum format = GL_RGB;

	/// <summary>
	/// The size of the render target in pixels
	/// </summary>
	glm::vec2 size = glm::vec2(0.0f);

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
	/// Resizes the render target. The render target should not be bound when resizing to prevent API errors.
	/// After the operation, the resized render target will be the active one, regardless of previous state.
	/// </summary>
	/// <param name="newSize">The new size of the render target</param>
	void resize(glm::vec2 newSize);

private:
	void attachTexture(TargetType targetTextureType, glm::vec2 size);
};