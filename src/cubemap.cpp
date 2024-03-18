#include <iostream>
#include <string>
#include <vector>

#include <stb_image.h>

#include "cubemap.hpp"

Cubemap::Cubemap()
{
	this->texID = {};
}

Cubemap::Cubemap(std::string facesPath)
{
	this->texID = {};

	faces.push_back(facesPath + "right.png");
	faces.push_back(facesPath + "left.png");
	faces.push_back(facesPath + "top.png");
	faces.push_back(facesPath + "bottom.png");
	faces.push_back(facesPath + "front.png");
	faces.push_back(facesPath + "back.png");
}

Cubemap::Cubemap(std::vector<std::string> faces)
{
	this->texID = {};
	this->faces = faces;
}

Cubemap::~Cubemap()
{
	glDeleteTextures(1, &this->texID);
}

void Cubemap::setupObject()
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