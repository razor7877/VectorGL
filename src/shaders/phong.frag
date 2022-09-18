#version 330 core
in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

vec3 phong()
{
	float viewDistance = length(lightPos - FragPos);
	float falloff = 1.0 / (1.0 + 0.01 * viewDistance*viewDistance);

	// Ambient lighting
	float ambientStrength = 0.15;
	vec3 ambient = ambientStrength * material.ambient;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	// Diffuse lighting
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lightColor * (diff * material.diffuse) * falloff;
	
	// Specular lighting
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = lightColor * (spec * material.specular) * falloff;

	vec3 result = ambient + diffuse + specular;
	return result;
}

void main()
{
	// If normals are supplied, apply phong lighting
	if (Normal != 0)
	{
		FragColor = vec4(phong(), 1.0);
	}
	else
	{
		FragColor = vec4(1.0);
	}

	// If texture coordinates are supplied, apply the texture
	if (TexCoord != 0)
	{
		FragColor *= texture(texture_diffuse1, TexCoord);
	}
}