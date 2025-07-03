#pragma once

#include "utilities/glad.h"

#include "texture.hpp"

/// <summary>
/// A TextureView allows wrapping a texture object from the graphics API without getting ownership.
/// This is useful whenever a texture may need to be accessed from multiple places but only one is responsible
/// for disposing of it.
/// </summary>
class TextureView
{
public:
    GLuint texID = 0;
    TextureType type = TextureType::TEXTURE_DIFFUSE;

    TextureView(GLuint id, TextureType type) : texID(texID), type(type) {}

    void bindTexture() const
    {
        if (this->type == TextureType::TEXTURE_3D)
            glBindTexture(GL_TEXTURE_2D_ARRAY, this->texID);
        else
		    glBindTexture(GL_TEXTURE_2D, this->texID);
    }
};
