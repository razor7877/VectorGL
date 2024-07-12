#include <iostream>
#include <string>
#include <vector>

#include <utilities/stb_image.h>

#include "cubemap.hpp"

Cubemap::Cubemap()
{
	this->texID = {};
}

Cubemap::Cubemap(std::string facesPath)
{
	faces.push_back(facesPath + "right.png");
	faces.push_back(facesPath + "left.png");
	faces.push_back(facesPath + "top.png");
	faces.push_back(facesPath + "bottom.png");
	faces.push_back(facesPath + "front.png");
	faces.push_back(facesPath + "back.png");
}

Cubemap::Cubemap(std::vector<std::string> faces)
{
	this->faces = faces;
}

Cubemap::Cubemap(GLuint cubemap)
{
	this->texID = cubemap;
	this->isSetUp = true;
}

Cubemap::Cubemap(GLenum format, int width, int height)
{
	glGenTextures(1, &this->texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->texID);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
			width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Cubemap::~Cubemap()
{
	glDeleteTextures(1, &this->texID);
}

void Cubemap::bind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->texID);
}

void Cubemap::setupObject()
{
	if (!this->isSetUp)
		this->createCubemapFromFaces();
}

void Cubemap::createCubemapFromFaces()
{
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	int width, height, nrChannels;
	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum format = GL_RGB;
			if (nrChannels == 1)
				format = GL_RED;
			if (nrChannels == 3)
				format = GL_RGB;
			if (nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Failed to load cubemap texture at path: " << faces[i] << std::endl;
		}

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}