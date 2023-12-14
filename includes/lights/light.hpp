#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

enum class LightType
{
	LIGHT_DIRLIGHT,
	LIGHT_POINTLIGHT,
	LIGHT_SPOTLIGHT
};

// The light class is an interface implemented by other classes (directional lights, point light, spotlights)
// Each light object parameters can then be sent to shaders to calculate lighting
class Light
{
public:
	// The colors of each of the light's components
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	Light();
	Light(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor);
	// Sends the necessary data as uniforms to a given program ID and a given index for the light type's array
	void virtual sendToShader(unsigned int shaderProgramID, unsigned int index) = 0;
	// Returns an enum corresponding to the specific class of a Light object
	// Used by the LightManager class to count the number of each type of light (for sending data to shaders)
	LightType virtual getLightType() = 0;
};

#endif